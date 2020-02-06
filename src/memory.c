/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2020, Peter Haemmerlein (peterpawn@yourfritz.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, please look for the file LICENSE.
 */

#define MEMORY_C

#include "common.h"

// memory buffer oriented functions

static	size_t		memoryBufferSize = DEFAULT_MEMORY_BUFFER_SIZE;

EXPORTED	void	memoryBufferSetSize(size_t newSize)
{
	memoryBufferSize = newSize;
}

// create a new buffer

EXPORTED	memoryBuffer_t *	memoryBufferNew(size_t size)
{
	memoryBuffer_t	*new = (memoryBuffer_t *) malloc(size);
	if (new)
	{
		memset(new, 0, size);
		new->size = size;
	}
	return new;
}

// free all buffers in a chain

EXPORTED	memoryBuffer_t *	memoryBufferFreeChain(memoryBuffer_t *start)
{
	memoryBuffer_t	*current = start;

	while (current)
	{
		memoryBuffer_t	*next = current->next;

		/* alternative: clearMemory(current, current->size, true); */
		memset(current, 0, current->size);
		free(current);

		current = next;
	}
	return NULL;
}

// read a file into one or more memory buffers and return the head element

EXPORTED	memoryBuffer_t *	memoryBufferReadFile(FILE * file, size_t chunkSize)
{
	size_t			allocSize;
	memoryBuffer_t	*top = NULL;
	memoryBuffer_t	*current = NULL;
	char *			data;
	size_t			read;
	size_t			toRead;

	allocSize = (chunkSize != (size_t) -1 ? chunkSize : memoryBufferSize);
	allocSize += sizeof(memoryBuffer_t);
	top = memoryBufferNew(allocSize);
	current = top;
	data = top->data;
	toRead = current->size - current->used - sizeof(memoryBuffer_t);
	while ((read = fread(data, 1, toRead, file)) > 0)
	{
		current->used += read;
		data += read;

		if ((read == toRead) && (current->used == current->size - sizeof(memoryBuffer_t))) /* buffer is full */
		{
			memoryBuffer_t	*next = memoryBufferNew(allocSize);

			if (!next)
			{
				setError(STDIN_BUFFER_ERR);
				top = memoryBufferFreeChain(top);
				break;
			}

			current->next = next;
			next->prev = current;
			current = next;
			data = current->data;
			toRead = current->size - sizeof(memoryBuffer_t);
		}
	}

	if (top)
	{
		if (current->prev && !current->used) /* no data in last buffer, previous was exactly filled to its end */
		{
			current->prev->next = NULL;
			memoryBufferFreeChain(current);
		}

		if (!top->used)	/* no data read at all */
		{
			setError(NOERROR);
			memoryBufferFreeChain(top);
			top = NULL;
		}
	}

	return top;
}

// compute the size of data stored in a memory buffer chain

EXPORTED 	size_t	memoryBufferDataSize(memoryBuffer_t * top)
{
	size_t			size = 0;
	memoryBuffer_t	*current = top;

	while (current)
	{
		size += current->used;
		current = current->next;
	}

	return size;
}

// consolidate the data from the specified buffer chain into a single buffer
// even if data is already in a single buffer, a copy will be created

EXPORTED	memoryBuffer_t *	memoryBufferConsolidateData(memoryBuffer_t * start)
{
	size_t			size = memoryBufferDataSize(start);
	memoryBuffer_t	*current = start;
	memoryBuffer_t	*new = memoryBufferNew(size + sizeof(memoryBuffer_t));

	if (!new) return NULL;

	char *			out = new->data;

	while (current)
	{
		memcpy(out, current->data, current->used);
		out += current->used;
		current = current->next;
	}

	new->used = size;

	return new;
}

// find a string in the buffer chain, handles crossing buffer borders

EXPORTED	char *	memoryBufferFindString(memoryBuffer_t * *buffer, size_t *offset, char *find, size_t findSize, bool *split)
{
	memoryBuffer_t	*current = *buffer;

	while (current)
	{
		char *		chk = current->data + *offset;
		size_t		remaining = current->used - *offset;

		while (remaining > 0)
		{
			if (remaining < findSize)
			{
				if (strncmp(chk, find, remaining) == 0)
				{
					if (current->next)
					{
						if (strncmp(current->next->data, find + remaining, findSize - remaining) == 0) /* match across buffers */
						{
							*buffer = current;
							*offset = (chk - current->data);
							*split = true;
							return chk;
						}
					}
					else /* partial match at end of data */
						return NULL;
				}
				chk++;
				remaining--;
				if (remaining == 0) /* next buffer, if any */
				{
					if (current->next)
					{
						current = current->next;
						remaining = current->used;
						chk = current->data;
					}
					else /* no match */
						return NULL;
				}
			}
			else
			{
				if (strncmp(chk, find, findSize) == 0)
				{
					*buffer = current;
					*offset = (chk - current->data);
					*split = false;
					return chk;
				}
				chk++;
				remaining--;
			}
		}
	}
	return NULL;
}

// advance a pointer value, following the buffer chain, if a buffer is exhausted

EXPORTED	char *	memoryBufferAdvancePointer(memoryBuffer_t * *buffer, size_t *lastOffset, size_t offset)
{
	memoryBuffer_t	*current = *buffer;
	size_t			advance = offset;

	while (current)
	{
		if ((current->used - *lastOffset) < advance) /* next buffer */
		{
			if (current->next)
			{
				advance -= (current->used - *lastOffset);
				current = current->next;
				*buffer = current;
				*lastOffset = 0;
				continue;
			}
		}

		*lastOffset += advance;

		return (current->data + *lastOffset);
	}

	return NULL;
}

// find the first non-Base32 character from the specified pointer

EXPORTED	char *	memoryBufferSearchValueEnd(memoryBuffer_t * *buffer, size_t *offset, size_t * size, bool *split)
{
	memoryBuffer_t	*current = *buffer;
	size_t			currentOffset = *offset;
	size_t			count = 0;
	char *			position = current->data + currentOffset;

	*split = false;
	while (current)
	{
		if (current->used <= currentOffset) /* next buffer */
		{
			if (current->next)
			{
				current = current->next;
				currentOffset = 0;
				position = current->data;
				*split = true;
			}
		}

		if ((*position >= 'A' && *position <= 'Z') || (*position >= '1' && *position <= '6'))
		{
			count++;
			position++;
			currentOffset++;
		}
		else /* character outside of our Base32 set found */
		{
			*buffer = current;
			*offset = currentOffset;
			*size = count;

			return position;
		}
	}

	return NULL;	
}

// scan memory buffer and replace occurrences of encrypted data while writing data to output;
// if no output file is used (NULL), input data has to be contained in a single buffer and cipher-text
// values are replaced with the corresponding clear-text in this buffer ... gaps are marked with 0xFF
// and a 16 bit integer containing the offset from 0xFF to the next valid character

EXPORTED	bool	memoryBufferProcessFile(memoryBuffer_t * *buffer, size_t offset, char * key, FILE * out, UNUSED char * filesKey)
{
	CipherContext 		*ctx = CipherInit(NULL, CipherTypeValue, NULL, NULL, false);
	memoryBuffer_t 		*current = *buffer;
	size_t				currentOffset = offset;
	
	while (current)
	{
		memoryBuffer_t	*found = current;
		size_t			foundOffset = currentOffset;
		bool			split = false;
		char *			cipherTextStart;
		char *			outputStart = NULL;

		if ((cipherTextStart = memoryBufferFindString(&found, &foundOffset, "$$$$", 4, &split)) != NULL) /* encrypted data exists */
		{
			if (!out)
				outputStart = cipherTextStart;

			while (current && (current != found)) /* output data crosses at least one buffer boundary */
			{
				if (out && fwrite(current->data + currentOffset, current->used - currentOffset, 1, out) != 1)
				{
					setError(WRITE_FAILED);
					current = NULL;
					break;
				}

				current = current->next;
				currentOffset = 0;
			}
			if (current && foundOffset > 0)
			{
				if (out && fwrite(current->data + currentOffset, foundOffset - currentOffset, 1, out) != 1)
				{
					setError(WRITE_FAILED);
					current = NULL;
					break;
				}

				currentOffset = foundOffset;
			}
			cipherTextStart = memoryBufferAdvancePointer(&current, &currentOffset, 4);
			found = current;
			foundOffset = currentOffset;

			size_t		valueSize;
			char *		cipherTextEnd = memoryBufferSearchValueEnd(&found, &foundOffset, &valueSize, &split);
			if (cipherTextEnd)
			{
				char *	copy;
				char *	cipherText = (char *) malloc(valueSize + 1);

				memset(cipherText, 0, valueSize + 1);
				copy = cipherText;
				while (current && (current != found))
				{
					memcpy(copy, current->data + currentOffset, current->used - currentOffset);
					copy += (current->used - currentOffset);
					current = current->next;
					currentOffset = 0;
				}

				memcpy(copy, current->data + currentOffset, foundOffset - currentOffset);
				*(cipherText + valueSize) = 0;

				if (!decryptValue(ctx, cipherText, valueSize, out, (out ? NULL : outputStart), key, true)) /* unable to decrypt, write data as is */
				{
					if (out)
					{
						if (fwrite("$$$$", 4, 1, out) != 1 ||
							fwrite(current->data + currentOffset, foundOffset - currentOffset, 1, out) != 1)
						{
							setError(WRITE_FAILED);
							current = NULL;
						}
						else
						{
							current = found;
							currentOffset = foundOffset;
						}
					}
				}
				else
				{
					if (!out)	/* set number of bytes to skip to get the next valid position */
					{
						while (*outputStart != '\xFF')
						{
							outputStart++;
						}
						*((uint16_t *) ((char *) outputStart + 1)) = (uint16_t) ((found->data + foundOffset) - outputStart);
					}
					current = found;
					currentOffset = foundOffset;
				}

				free(cipherText);
			}
		}
		else /* no more encrypted data, write remaining buffers */
		{
			if (out)
			{
				while (current)
				{
					if (fwrite(current->data + currentOffset, current->used - currentOffset, 1, out) != 1)
					{
						setError(WRITE_FAILED);
						current = NULL;
						break;
					}

					current = current->next;
					currentOffset = 0;
				}
			}
			else
				break;
		}
	}

	ctx = CipherCleanup(ctx);

	return !isAnyError();

}

// free memory after clearing its content

void *	clearMemory(void * buffer, size_t size, bool freeBuffer)
{
	if (buffer)
	{
		memset(buffer, 0, size);

		if (freeBuffer) free(buffer);
	}

	return NULL;
}
