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

#define PKPWD_C

#include "common.h"
#include "pkpwd_usage.c"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wformat-security"

static	char *				__commandNames[] = {
#include "pkpwd_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static	commandEntry_t 		__pkpwd_command = { .names = &commandNames, .ep = &pkpwd_entry, .usage = &pkpwd_usage, .short_desc = &pkpwd_shortdesc, .usesCrypto = true, .finalNewlineOnTTY = true };
EXPORTED commandEntry_t *	pkpwd_command = &__pkpwd_command;

// translation table

// 'privatekeypassword' function - compute the secret key for the private key file websrv_ssl_key.pem

int		pkpwd_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	char				out[8 + 1];
	size_t				outLen = sizeof(out);
	bool				altEnv = false;
	char *				maca = NULL;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			altenv_options_long,
			options_long_end,
		};
		char *			options_short = ":" altenv_options_short verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				check_altenv_options_short();
				check_verbosity_options_short();
				help_option();
				getopt_invalid_option();
				invalid_option(opt);
			}
		}
		if (optind < (argc - argo))
		{
			maca = argv[optind + argo];
			warnAboutExtraArguments(argv, optind + argo + 1);
		}
	}
	
	if (maca)
	{
		if (!checkMACAddress(maca))
		{
			errorMessage(errorWrongMACAddress, maca);
			setError(INV_HEX_DATA);
		}

		if (!isAnyError() && altEnv)
		{
			warningMessage(verboseAltEnvIgnored);
			failOnStrict();
		}
	}
	else
	{
		altenv_verbose_message();

		maca = getEnvironmentValue(NULL, URLADER_MACA_NAME);
		if (!maca)
		{
			errorMessage(errorMissingDeviceProperty, URLADER_MACA_NAME);		
		}
	}

	if (isAnyError())
		return EXIT_FAILURE;

	resetError();

	verboseMessage(verboseMACUsed, maca);

	if (privateKeyPassword(out, &outLen, maca))
	{
		if (fwrite(out, outLen, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			errorMessage(errorWriteFailed);
		}
	}

	return (!isAnyError() ? EXIT_SUCCESS : EXIT_FAILURE);
}

#pragma GCC diagnostic pop
