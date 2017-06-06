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

#define HEXENC_C

#include "common.h"

static commandEntry_t 		__hexenc_command = { .name = "hexenc", .ep = &hexenc_entry, .usage = &hexenc_usage };
EXPORTED commandEntry_t *	hexenc_command = &__hexenc_command;

// display usage help

void 	hexenc_usage(bool help)
{
	errorMessage("help for hexenc\n");
	if (help)
		errorMessage("option --help used\n");
}

// 'hexenc' function - encode binary data from STDIN to its hexadecimal presentation on STDOUT

int hexenc_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	bool				wrapLines = false;
	uint32_t			lineSize = 80;
	uint32_t			charsOnLine = 0;
	char				buffer[120];
	size_t				read = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "wrap-lines", optional_argument, NULL, 'w' },
		};
		char *			options_short = "w" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'w':
					{
						char *	endString = NULL;
						char *	startString;

						wrapLines = true;
						if (optarg && *optarg)
						{
							startString = optarg;	
						}
						else
						{
							if ((optind + argo) >= argc)
								break; /* last option, no number present */
							startString = argv[optind + argo];
							if (*startString == '-')
								break; /* next is an option */
						}
						lineSize = strtoul(startString, &endString, 10);
						if (*startString && strlen(endString))
						{
							errorMessage("Invalid line size '%s' specified for -w option.\a\n", startString);
							return(EXIT_FAILURE);
						}
						else
							optind++;
					}
					break;

				check_verbosity_options_short();
				help_option();
				getopt_message_displayed();
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
	{
		char		output[(sizeof(buffer) * 2) + 2]; /* one more byte for optional end of string */
		size_t		outputSize = binaryToHexadecimal(buffer, read, output, sizeof(output) - 1);
		
		if (outputSize == 0) break;
		resetError();
		
		uint32_t	toWrite = outputSize;
		char *		out = output;

		out = wrapOutput(wrapLines, lineSize, &charsOnLine, &toWrite, out);
		if (isAnyError())
			break;
		
		if ((toWrite > 0) && fwrite(out, toWrite, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			break;
		}
		if (wrapLines)
		{
			charsOnLine += toWrite;
		}
	}
	if (!isAnyError() && wrapLines && (fwrite("\n", 1, 1, stdout) != 1)) /* append newline */
		setError(WRITE_FAILED);
	
	if (isAnyError()) 
	{
		if (isError(WRITE_FAILED))
		{
			errorMessage("Write to STDOUT failed.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
