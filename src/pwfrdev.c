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

#define PWFRDEV_C

#include "common.h"
#include "pwfrdev_usage.c"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wformat-security"

static	char *				__commandNames[] = {
#include "pwfrdev_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static	commandEntry_t 		__pwfrdev_command = { .names = &commandNames, .ep = &pwfrdev_entry, .usage = &pwfrdev_usage, .short_desc = &pwfrdev_shortdesc, .usesCrypto = true };
EXPORTED commandEntry_t *	pwfrdev_command = &__pwfrdev_command;

// 'password_from_device' function - compute the password hash from the current device properties

int		pwfrdev_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	bool				hexOutput = false;
	bool				forExport = false;
	char				hash[MAX_DIGEST_SIZE];
	size_t				hashLen = sizeof(hash);
	char				hex[(sizeof(hash) * 2) + 1];
	size_t				hexLen = 0;
	char *				out;
	size_t				outLen;
	bool				altEnv = false;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			altenv_options_long,
			{ "hex-output", no_argument, NULL, 'x' },
			{ "for-export", no_argument, NULL, 'e' },
			options_long_end,
		};
		char *			options_short = ":" "xe" altenv_options_short verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexOutput = true;
					break;

				case 'e':
					forExport = true;
					break;

				check_altenv_options_short();
				check_verbosity_options_short();
				help_option();
				getopt_argument_missing();
				getopt_invalid_option();
				invalid_option(opt);
			}
		}
		if (optind < (argc - argo))
		{
			warnAboutExtraArguments(argv, optind + argo);
		}
	}

	if (isAnyError())
		return EXIT_FAILURE;

	resetError();

	altenv_verbose_message();

	keyFromDevice(hash, &hashLen, forExport);

	if (!isAnyError())
	{
		hexLen = binaryToHexadecimal((char *) hash, hashLen, hex, sizeof(hex));
		hex[hexLen] = 0;
		verboseMessage(verboseDeviceKeyHash, hex);
	}

	if (!isAnyError() && !hexOutput && isatty(1))
	{
		if (fwrite("0x", 2, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			errorMessage(errorWriteFailed);
		}
		hexOutput = true;
	}
	
	if (!isAnyError())
	{
		if (hexOutput)
		{
			outLen = binaryToHexadecimal((char *) hash, hashLen, hex, sizeof(hex));
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
	}
	
	return (!isAnyError() ? EXIT_SUCCESS : EXIT_FAILURE);
}

#pragma GCC diagnostic pop
