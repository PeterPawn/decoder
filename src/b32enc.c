/*
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2018, Peter Haemmerlein (peterpawn@yourfritz.de)
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

#define B32ENC_C

#include "common.h"
#include "b32enc_usage.c"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wformat-security"

static	char *				__commandNames[] = {
#include "b32enc_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static	commandEntry_t 		__b32enc_command = { .names = &commandNames, .ep = &b32enc_entry, .usage = &b32enc_usage, .short_desc = &b32enc_shortdesc, .finalNewlineOnTTY = true };
EXPORTED commandEntry_t *	b32enc_command = &__b32enc_command;

// 'b32enc' function - encode binary data from STDIN to Base32 encoded on STDOUT

int 	b32enc_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	bool				hexInput = false;
	bool				padInput = false;
	char				buffer[20];
	size_t				read = 0;
	size_t				charsOnLine = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			{ "hex-input", no_argument, NULL, 'x' },
			{ "pad-input", no_argument, NULL, 'p' },
			width_options_long,
			verbosity_options_long,
			options_long_end,
		};
		char *			options_short = ":" "xp" width_options_short verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexInput = true;
					break;

				case 'p':
					padInput = true;
					break;

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

	if (isAnyError())
		return EXIT_FAILURE;

	resetError();

	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
	{
		if (hexInput)
		{
			char		withoutSpaces[sizeof(buffer)];
			size_t		used = 0;
			char *		in;
			char *		out;
			int			i;
			size_t		more = read;

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
			if (read == 0) 
				break;
		}
		if ((read % 5))
		{
			if (padInput)
			{
				int		r = 5 - (read % 5);
				char *	pad = &buffer[read];

				for (int i = 0; i < r; i++)
				{
					*pad++ = 0;
					read++;
				}
			}
			else
			{
				setError(INV_B32_ENC_SIZE);
				read = 0;
				break;	
			}
		}
		
		char			base32[(sizeof(buffer) / 5 * 8) + 1];
		size_t			base32Size = binaryToBase32(buffer, read, base32, sizeof(base32) - 1);

		if (base32Size > 0)
		{
			char *		out = base32;

			out = wrapOutput(stdout, &charsOnLine, &base32Size, base32);
			if (fwrite(out, base32Size, 1, stdout) != 1)
			{
				setError(WRITE_FAILED);
				errorMessage(errorWriteFailed);
				return EXIT_FAILURE;
			}
			charsOnLine += base32Size;
		}
	}
	
	if (isAnyError()) 
	{
		if (isError(INV_HEX_DATA))
		{
			errorMessage(errorInvalidHexValue);
		}
		else if (isError(INV_HEX_SIZE))
		{
			errorMessage(errorInvalidHexSize);
		}
		else if (isError(INV_B32_ENC_SIZE))
		{
			errorMessage(errorInvalidDataSize);
		}
		else
		{
			errorMessage(errorUnexpectedError, getError(), getErrorText(getError()));
		}
	}

	return (!isAnyError() ? EXIT_SUCCESS : EXIT_FAILURE);
}

#pragma GCC diagnostic pop
