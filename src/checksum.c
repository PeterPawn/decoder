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
static	commandEntry_t 		__checksum_command = { .names = &commandNames, .ep = &checksum_entry, .short_desc = &checksum_shortdesc, .usage = &checksum_usage };
EXPORTED commandEntry_t *	checksum_command = &__checksum_command;

// 'checksum' function - compute the CRC32 checksum for STDIN data

int		checksum_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	crcCtx_t *			ctx = NULL;
	char				buffer[256];
	size_t				read = 0;
	uint32_t			value = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			options_long_end,
		};
		char *			options_short = ":" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
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

	ctx = crcInit();

	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
	{
		crcUpdate(ctx, buffer, read);
	}
	
	value = crcFinal(ctx);

	if (isAnyError()) 
	{
		if (isError(WRITE_FAILED))
		{
			errorMessage(errorWriteFailed);
		}
		else if (isError(INV_HEX_DATA))
		{
			errorMessage(errorInvalidHexValue);
		}
		else if (isError(INV_HEX_SIZE))
		{
			errorMessage(errorInvalidHexSize);
		}
		else if (isError(INV_B64_ENC_SIZE))
		{
			errorMessage(errorInvalidDataSize);
		}
		else
		{
			errorMessage(errorUnexpectedError, getError(), getErrorText(getError()));
		}
	}

	fprintf(stdout, "%08X", value);

	return (!isAnyError() ? EXIT_SUCCESS : EXIT_FAILURE);
}

#pragma GCC diagnostic pop
