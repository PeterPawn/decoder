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

#define USERPW_C

#include "common.h"
#include "userpw_usage.c"

static	char *				__commandNames[] = {
#include "userpw_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static	commandEntry_t 		__userpw_command = { .names = &commandNames, .ep = &userpw_entry, .usage = &userpw_usage, .usesCrypto = true };
EXPORTED commandEntry_t *	userpw_command = &__userpw_command;

// 'user_password' function - compute the password hash for export files with a user-specified password

int		userpw_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	bool				hexOutput = false;
	char *				password = NULL;
	char				hash[MAX_DIGEST_SIZE];
	uint32_t			hashLen = sizeof(hash);
	char				hex[(sizeof(hash) * 2) + 1];
	size_t				hexLen;
	char *				out;
	size_t				outLen;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-output", no_argument, NULL, 'x' },
			options_long_end,
		};
		char *			options_short = ":" "x" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexOutput = true;
					break;

				check_verbosity_options_short();
				help_option();
				getopt_message_displayed();
				invalid_option(opt);
			}
		}
		fprintf(stderr, "optint=%u, argc=%u, argo=%u optIndex=%u\n", optind, argc, argo, optIndex);
		if (optind >= (argc - argo))
		{
			errorMessage(errorPasswordMissing);
			__autoUsage();
			return EXIT_FAILURE;
		}
		else
		{
			password = argv[optind + argo];
			warnAboutExtraArguments(argv, optind + argo);
		}
	}
	else
	{
		errorMessage(errorPasswordMissing);
		__autoUsage();
		return EXIT_FAILURE;
	}

	resetError();

	if ((hashLen = Digest(password, strlen(password), hash, hashLen)) == 0)
	{
		errorMessage(errorDigestComputation);
		return EXIT_FAILURE;
	}

	hexLen = binaryToHexadecimal((char *) hash, hashLen, hex, sizeof(hex));
	hex[hexLen] = 0;
	verboseMessage(verbosePasswordHash, hex);
	if (hexOutput)
	{
		outLen = hexLen;
		out = hex;
		entry->finalNewlineOnTTY = true;
	}
	else
	{
		outLen = hashLen;
		out = (char *) hash;
	}
	if (fwrite(out, outLen, 1, stdout) != 1)
	{
		setError(WRITE_FAILED);
		errorMessage(errorWriteFailed);
	}

	return (!isAnyError() ? EXIT_SUCCESS : EXIT_FAILURE);
}
