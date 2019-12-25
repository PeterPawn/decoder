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

#define DECOMPOSE_C

#include "common.h"
#include "decompose_usage.c"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wformat-security"

static	char *				__commandNames[] = {
#include "decompose_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static	commandEntry_t 		__decompose_command = { .names = &commandNames, .ep = &decompose_entry, .short_desc = &decompose_shortdesc, .usage = &decompose_usage };
EXPORTED commandEntry_t *	decompose_command = &__decompose_command;

// 'decompose' function - split an export file into single files

int		decompose_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	char *				outputDir = NULL;
	struct stat			fileStat;
	bool				withDictionary = false;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			{ "output-directory", required_argument, NULL, 'o' },
			{ "dictionary", required_argument, NULL, 'd' },
			verbosity_options_long,
			options_long_end,
		};
		char *			options_short = ":" "o:d" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'o':
					{
						if (!optarg)
						{
							errorMessage(errorMissingDirectoryName);
							setError(OPTION_VALUE_MISSING);
							return EXIT_FAILURE;
						}
						outputDir = strdup(optarg);
					}
					break;

				case 'd':
					withDictionary = true;
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

	if (!outputDir)
	{
		errorMessage(errorMissingDirectoryName);
		setError(OPTION_VALUE_MISSING);
		return EXIT_FAILURE;
	}

	if (stat(outputDir, &fileStat) != 0 || !S_ISDIR(fileStat.st_mode))
	{
		errorMessage(errorInvalidDirectoryName, outputDir);
		setError(OPTION_VALUE_INVALID);
	}

	if (isAnyError())
		return EXIT_FAILURE;

	resetError();

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

	decomposeExportFile(inputFile, outputDir, withDictionary);

	memoryBufferFreeChain(inputFile);

	return (!isAnyError() ? EXIT_SUCCESS : EXIT_FAILURE);
}

#pragma GCC diagnostic pop
