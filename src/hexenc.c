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
#include "hexenc_usage.c"

static	char *				__commandNames[] = {
#include "hexenc_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static	commandEntry_t 		__hexenc_command = { .names = &commandNames, .ep = &hexenc_entry, .usage = &hexenc_usage, .finalNewlineOnTTY = true };
EXPORTED commandEntry_t *	hexenc_command = &__hexenc_command;

// 'hexenc' function - encode binary data from STDIN to its hexadecimal presentation on STDOUT

int		hexenc_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	size_t				charsOnLine = 0;
	char				buffer[120];
	size_t				read = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			width_options_long,
			verbosity_options_long,
			options_long_end,
		};
		char *			options_short = ":" width_options_short verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				check_width_options_short();
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

	resetError();

	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
	{
		char			output[(sizeof(buffer) * 2) + 2]; /* one more byte for optional end of string */
		size_t			outputSize = binaryToHexadecimal(buffer, read, output, sizeof(output) - 1);
		
		if (outputSize == 0) break;
		resetError();
		
		size_t			toWrite = outputSize;
		char *			out = output;

		out = wrapOutput(stdout, &charsOnLine, &toWrite, out);
		if (isAnyError())
			break;
		
		if ((toWrite > 0) && fwrite(out, toWrite, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			break;
		}
		charsOnLine += toWrite;
	}
	if (!isAnyError())
		wrapOutput(stdout, &charsOnLine, NULL, NULL);
	
	if (isAnyError()) 
	{
		if (isError(WRITE_FAILED))
		{
			errorMessage(errorWriteFailed);
		}
		else
		{
			errorMessage(errorUnexpectedError, getError(), getErrorText(getError()));
		}
	}

	return (!isAnyError() ? EXIT_SUCCESS : EXIT_FAILURE);
}
