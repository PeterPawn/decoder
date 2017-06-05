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

#define HEXDEC_C

#include "common.h"

static commandEntry_t 		__hexdec_command = { .name = "hexdec", .ep = &hexdec_entry, .usage = &hexdec_usage };
EXPORTED commandEntry_t *	hexdec_command = &__hexdec_command;

// display usage help

void 	hexdec_usage(bool help)
{
	errorMessage("help for hexdec\n");
	if (help)
		errorMessage("option --help used\n");
}

// 'hexdec' function - decode hexadecimal presentation of data from STDIN to STDOUT

int hexdec_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	char				buffer[80];
	size_t				read = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
		};
		char *			options_short = verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
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
		char	withoutSpaces[sizeof(buffer)];
		size_t	used = 0;
		char *	in;
		char *	out;
		int		i;
		size_t	more = read;

		in = buffer;
		out = withoutSpaces;
		while (more > 0)
		{
			for (i = more; i > 0; i--, in++)
			{
				if (isspace(*(in)))
					continue;
				*(out++) = *in;
				used++;
			}
			if (used == sizeof(withoutSpaces))
				break;
			more = fread(buffer, 1, sizeof(withoutSpaces) - used, stdin);
			if (more == 0)
				break;
			in = buffer;
		}
		read = hexadecimalToBinary(withoutSpaces, used, buffer, sizeof(buffer));
		if (read == 0 || isAnyError()) 
			break;

		if (fwrite(buffer, read, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			break;
		}
	}
		
	if (isAnyError()) 
	{
		if (isError(WRITE_FAILED))
		{
			errorMessage("Write to STDOUT failed.\a\n");
		}
		else if (isError(INV_HEX_DATA))
		{
			errorMessage("Invalid hexadecimal data value encountered on STDIN.\a\n");
		}
		else if (isError(INV_HEX_SIZE))
		{
			errorMessage("Invalid hexadecimal data size encountered on STDIN.\a\n");
		}
		else if (isError(INV_B64_ENC_SIZE))
		{
			errorMessage("Invalid data size encountered on STDIN.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
