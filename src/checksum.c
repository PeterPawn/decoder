/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2019, Peter Haemmerlein (peterpawn@yourfritz.de)
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

#define CHECKSUM_C

#include "common.h"
#include "checksum_usage.c"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wformat-security"

static	char *				__commandNames[] = {
#include "checksum_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static	commandEntry_t 		__checksum_command = { .names = &commandNames, .ep = &checksum_entry, .short_desc = &checksum_shortdesc, .usage = &checksum_usage, .finalNewlineOnTTY = true };
EXPORTED commandEntry_t *	checksum_command = &__checksum_command;

// 'checksum' function - compute the CRC32 checksum for STDIN data

int		checksum_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	crcCtx_t *			ctx = NULL;
	char				buffer[256];
	size_t				read = 0;
	uint32_t			crcValue = 0;
	bool				allData = false;
	enum {
		OUTPUT_NONE,
		OUTPUT_DECIMAL,
		OUTPUT_HEXADECIMAL,
		OUTPUT_HOST,
		OUTPUT_REVERSE,
	} 					outputMode = OUTPUT_NONE;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			{ "all-data", no_argument, NULL, 'd' },
			{ "hex-output", no_argument, NULL, 'x' },
			{ "raw-output", no_argument, NULL, 'r' },
			{ "lsb-output", no_argument, NULL, 'l' },
			{ "msb-output", no_argument, NULL, 'm' },
			verbosity_options_long,
			options_long_end,
		};
		char *			options_short = ":dxrlm" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'd':
					allData = true;
					break;

				case 'x':
					if (outputMode != OUTPUT_NONE)
					{
						errorMessage(errorConflictingOptions);
						setError(OPTIONS_CONFLICT);
						return EXIT_FAILURE;
					}
					outputMode = OUTPUT_HEXADECIMAL;
					break;

				case 'r':
					if (outputMode != OUTPUT_NONE)
					{
						errorMessage(errorConflictingOptions);
						setError(OPTIONS_CONFLICT);
						return EXIT_FAILURE;
					}
					outputMode = OUTPUT_HOST;
					break;

				case 'l':
					if (outputMode != OUTPUT_NONE)
					{
						errorMessage(errorConflictingOptions);
						setError(OPTIONS_CONFLICT);
						return EXIT_FAILURE;
					}
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
					outputMode = OUTPUT_HOST;
#else
					outputMode = OUTPUT_REVERSE;
#endif
					break;

				case 'm':
					if (outputMode != OUTPUT_NONE)
					{
						errorMessage(errorConflictingOptions);
						setError(OPTIONS_CONFLICT);
						return EXIT_FAILURE;
					}
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
					outputMode = OUTPUT_HOST;
#else
					outputMode = OUTPUT_REVERSE;
#endif
					break;

				check_verbosity_options_short();
				help_option();
				getopt_invalid_option();
				invalid_option(opt);
			}
		} 
		if (optind < argc)
			warnAboutExtraArguments(argv, optind + 1);
	}

	if (isatty(0))
	{
		errorMessage(errorNoReadFromTTY);
		return EXIT_FAILURE;
	}

	if (isAnyError())
		return EXIT_FAILURE;

	resetError();

	if (allData)
	{
		ctx = crcInit();
	
		while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
		{
			crcUpdate(ctx, buffer, read);
		}

		crcValue = crcFinal(ctx);

		if (outputMode == OUTPUT_NONE)
			outputMode = OUTPUT_DECIMAL;
	}
	else
	{
		memoryBuffer_t	*inputFile = memoryBufferReadFile(stdin, -1);

		if (!inputFile)
		{
			if (!isAnyError()) /* empty input file */
			{
				errorMessage(errorEmptyInputFile);
			}
			else
			{
				errorMessage(errorReadToMemory);
			}
			return EXIT_FAILURE;
		}

		memoryBuffer_t	*consolidated = memoryBufferConsolidateData(inputFile);

		if (!consolidated)
		{
			errorMessage(errorNoMemory);
			inputFile = memoryBufferFreeChain(inputFile);
			return EXIT_FAILURE;
		}
		else
		{
			inputFile = memoryBufferFreeChain(inputFile);
			inputFile = consolidated;
			verboseMessage(verboseInputDataConsolidated, memoryBufferDataSize(inputFile));
		}

		crcValue = computeExportFileChecksum(inputFile, (outputMode == OUTPUT_NONE ? stdout : NULL));

		memoryBufferFreeChain(inputFile);

		if (outputMode == OUTPUT_NONE)
			entry->finalNewlineOnTTY = false;
	}

	if (!isAnyError())
	{
		switch (outputMode)
		{
			case OUTPUT_HEXADECIMAL:
				fprintf(stdout, "%08X", crcValue);
				break;

			case OUTPUT_REVERSE:
				/* swap 'value' in-place with XOR */
				*(((uint8_t *) &crcValue) + 0) ^= *(((uint8_t *) &crcValue) + 3);
				*(((uint8_t *) &crcValue) + 3) ^= *(((uint8_t *) &crcValue) + 0);
				*(((uint8_t *) &crcValue) + 0) ^= *(((uint8_t *) &crcValue) + 3);
				*(((uint8_t *) &crcValue) + 1) ^= *(((uint8_t *) &crcValue) + 2);
				*(((uint8_t *) &crcValue) + 2) ^= *(((uint8_t *) &crcValue) + 1);
				*(((uint8_t *) &crcValue) + 1) ^= *(((uint8_t *) &crcValue) + 2);
				/* fall through to next case */
				__attribute__ ((fallthrough));

			case OUTPUT_HOST:
				fwrite(&crcValue, sizeof(uint32_t), 1, stdout);
				break;

			case OUTPUT_DECIMAL:
				fprintf(stdout, "%u", crcValue);
				break;

			case OUTPUT_NONE:
				break;
		}
	}

	return (!isAnyError() ? EXIT_SUCCESS : EXIT_FAILURE);
}

#pragma GCC diagnostic pop
