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

#define B32DEC_C

#include "common.h"
#include "b32dec_usage.c"

static commandEntry_t 		__b32dec_command = { .name = "b32dec", .ep = &b32dec_entry, .usage = &b32dec_usage };
EXPORTED commandEntry_t *	b32dec_command = &__b32dec_command;

// statics

//// error messages ////
static	char *			errorInvalidValue = "Invalid data value encountered on STDIN.\n";
static	char *			errorInvalidDataSize = "Invalid data size encountered on STDIN.\n";
static	char *			errorUnexpectedError = "Unexpected error %d (%s) encountered.\n";
static	char *			errorWriteFailed = "Write to STDOUT failed.\n";
//// end ////

// 'b32dec' function - decode Base32 encoded data from STDIN to STDOUT

int 	b32dec_output(char * base32, bool hexOutput)
{
	char				binary[5];
	size_t				binarySize = base32ToBinary(base32, (size_t) -1, binary, sizeof(binary));
	char				hex[10];
	char *				out;
	size_t				outSize;

	if (isAnyError()) /* usually invalid characters */
	{
		if (isError(INV_B32_DATA))
		{
			errorMessage(errorInvalidValue);
		}
		else if (isError(INV_B32_SIZE))
		{
			errorMessage(errorInvalidDataSize);
		}
		else
		{
			errorMessage(errorUnexpectedError, getError(), getErrorText(getError()));
		}
		return EXIT_FAILURE;
	}
	if (hexOutput)
	{
		outSize = binaryToHexadecimal(binary, binarySize, hex, sizeof(hex));
		out = hex;
	}
	else
	{
		outSize = binarySize;
		out = binary;
	}
	if (fwrite(out, outSize, 1, stdout) != 1)
	{
		setError(WRITE_FAILED);
		errorMessage(errorWriteFailed);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int		b32dec_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	char				buffer[81];
	char *				input;
	char				base32[9];
	int					convUsed = 0;
	bool				hexOutput = false;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-output", no_argument, NULL, 'x' },
		};
		char *			options_short = "x" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexOutput = true;
					entry->finalNewlineOnTTY = true;
					break;

				check_verbosity_options_short();
				help_option();
				getopt_message_displayed();
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((input = fgets(buffer, sizeof(buffer), stdin)) != NULL)
	{
		input--;
		while (*(++input))
		{
			if (isspace(*input))
				continue;
			base32[convUsed++] = *input;
			if (convUsed == 8)
			{
				int		result;

				base32[convUsed] = 0;
				if ((result = b32dec_output(base32, hexOutput)))
					return result;
				convUsed = 0;
			}
		}
	}	

	if (convUsed > 0) /* remaining data exist */
	{
		base32[convUsed] = 0;
		return b32dec_output(base32, hexOutput);
	}
	
	return (!isAnyError() ? EXIT_SUCCESS : EXIT_FAILURE);
}
