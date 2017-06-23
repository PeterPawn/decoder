/*
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2017, Peter Haemmerlein (peterpawn@yourfritz.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, please look for the file LICENSE.
 */

#define CHKSUM_C

#include "common.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wformat-security"

// FRITZ!OS export file checksum routines

char *	getNextLine(memoryBuffer_t * input, size_t * offset, size_t * size)
{
	char *				current = input->data + *offset;
	char *				start;

	if (*current == '\xFF') /* skip the specified number of bytes */
	{
		uint16_t		skip = *((uint16_t *) (current + 1));

		current += skip;
		*offset = (current - input->data);
	}

	start = current;

	while (*current != '\n' && *current != '\xFF' && current < (input->data + input->used))
	{
		current++;
	}

	*size = (current - (input->data + *offset) + (*current == '\n' ? 1 : 0)); /* include newline at end */
	*offset += *size;

	return start;
}

EXPORTED	uint32_t	computeExportFileChecksum(memoryBuffer_t * input, FILE * out)
{
	char *				current;
	char *				last = NULL;
	size_t				offset = 0;
	size_t				size = 0;
	size_t				lastSize = 0;
	enum {
		NO_OUTPUT,
		IN_HEADER,
		IN_TEXTFILE,
		IN_BINFILE,
	}					output = NO_OUTPUT;
	uint32_t			crcValue = 0;
	char				buffer[9];

	crcCtx_t *			ctx = crcInit();

	while ((current = getNextLine(input, &offset, &size)) && (size > 0))
	{
		if (strncmp(current, "**** ", 5) == 0) /* any marker line */
		{
			if (strncmp(current + 5, "END OF FILE ****", 16) == 0) /* end of file found */
			{
				output = NO_OUTPUT;
			}
			else if (strncmp(current + 5, "END OF EXPORT ", 14) == 0) /* end of export file found */
			{
				memcpy(buffer, current + 5 + 14, 8);
				buffer[8] = 0;
				verboseMessage(verboseChecksumFound, buffer);

				crcValue = crcFinal(ctx);
				snprintf(buffer, sizeof(buffer), "%08X", crcValue);
				buffer[8] = 0;

				if (strncmp(current + 5 + 14, buffer, 8) == 0)
				{
					verboseMessage(verboseChecksumIsValid);
				}
				else
				{
					verboseMessage(verboseNewChecksum, buffer);
					if (out)
					{
						if ((fwrite(current, 5 + 14, 1, out) != 1) ||
							(fwrite(buffer, 8, 1, out) != 1) ||
							(fwrite(current + 5 + 14 + 8, size - (5 + 14 + 8), 1, out) != 1))
						{
							setError(WRITE_FAILED);
							break;
						}
						else
						{
							size = 0; /* all output was written already */
						}
					}
				}

				output = NO_OUTPUT;
			}
			else if (strncmp(current + 5, "FRITZ", 5) == 0) /* header start found */
			{
				output = IN_HEADER;
			}
			else if (strncmp(current + 5, "CFGFILE:", 8) == 0) /* text file found */
			{
				char *	value = current + 5 + 8;
				size_t	valueSize = size - (5 + 8) - (*(current + size - 1) == '\n' ? 1 : 0);

				output = IN_TEXTFILE;
				last = NULL;
				lastSize = 0;
				crcUpdate(ctx, value, valueSize);
				crcUpdate(ctx, "\0", 1);
			}
			else if (strncmp(current + 5, "BINFILE:", 8) == 0 || strncmp(current + 5, "CRYPTEDBINFILE:", 15) == 0) /* binary file found */
			{
				char *	value = current + 5 + (strncmp(current + 5, "BIN", 3) == 0 ? 8 : 15);
				size_t	valueSize = size - (5 + (strncmp(current + 5, "BIN", 3) == 0 ? 8 : 15)) - (*(current + size - 1) == '\n' ? 1 : 0);

				output = IN_BINFILE;
				crcUpdate(ctx, value, valueSize);
				crcUpdate(ctx, "\0", 1);
			}
		}
		else
		{
			switch (output)
			{
				case IN_HEADER:
					{
						/* count name and value, without equal-sign and with NUL instead of a final newline */

						char *	value = current;
						size_t	valueSize = 0;

						while ((*(value + valueSize) != '=') && ((value + valueSize) < (current + size)))
						{
							valueSize++;
						}

						if (*(value + valueSize) == '=')
						{
							crcUpdate(ctx, value, valueSize); /* name */
							value += valueSize + 1;
							valueSize = 0;

							while ((*(value + valueSize) != '\n') && ((value + valueSize) < (current + size)))
							{
								valueSize++;
							}

							if (*(value + valueSize) == '\n')
							{
								crcUpdate(ctx, value, valueSize); /* value */
								crcUpdate(ctx, "\0", 1);
							}
						}
					}
					break;

				case IN_TEXTFILE:
					{
						/* do not count the last line ... that's why counting will be delayed to the next call */
						/* double backslashes are counted as single ones */

						char *	value = last;
						size_t	valueSize = lastSize;

						if (value)
						{
							while (valueSize > 0)
							{
								while (valueSize > 0 && *value != '\\')
								{
									value++;
									valueSize--;
								}

								if (*value == '\\' && *(value + 1) == '\\' && valueSize >= 2)
								{
									crcUpdate(ctx, last, lastSize - valueSize);
									last = value + 1;
									lastSize = valueSize - 1;
									value += 2;
									valueSize -= 2;
								}
							}

							if (lastSize > 0)
							{
								crcUpdate(ctx, last, lastSize);
							}
						}

						last = current;
						lastSize = size;
					}
					break;

				case IN_BINFILE:
					{
						/* count each decoded line (convert it to binary first) */

						char *	binary = malloc((size + 1) / 2);
						size_t	binSize = (size + 1) / 2;

						if (binary)
						{
							binSize = hexadecimalToBinary(current, size, binary, binSize);
							crcUpdate(ctx, binary, binSize);
							free(binary);
						}
					}
					break;

				default:
					break;
			}
		}

		if (out && size)
		{
			if (fwrite(current, size, 1, out) != 1)
			{
				setError(WRITE_FAILED);
				break;
			}
		}
	}

	return crcValue;
}

#pragma GCC diagnostic pop
