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

#define DECFILE_C

#include "common.h"
#include "decfile_usage.c"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wformat-security"

static	char *				__commandNames[] = {
#include "decfile_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static	commandEntry_t 		__decfile_command = { .names = &commandNames, .ep = &decfile_entry, .usage = &decfile_usage, .short_desc = &decfile_shortdesc, .usesCrypto = true };
EXPORTED commandEntry_t *	decfile_command = &__decfile_command;

// 'decode_secrets' function - decode all secret values from STDIN content and copy it with replaced values to STDOUT

int		decfile_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	char 				hash[MAX_DIGEST_SIZE];
	size_t				hashLen = sizeof(hash);
	char *				serial = NULL;
	char *				maca = NULL;
	char *				wlanKey = NULL;
	char *				tr069Passphrase = NULL;
	bool				altEnv = false;
	bool				tty = false;
	size_t				hexLen = 0;
	char *				hexBuffer = NULL;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			{ "tty", no_argument, NULL, 't' },
			{ "block-size", required_argument, NULL, 'b' },
			altenv_options_long,
			verbosity_options_long,
			options_long_end,
		};
		char *			options_short = ":" "tb:" altenv_options_short verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 't':
					tty = true;
					break;

				case 'b':
					if (!setInputBufferSize(optarg, argv[optind]))
					{
						setError(INV_BUF_SIZE);
						return EXIT_FAILURE;
					}
					break;

				check_altenv_options_short();
				check_verbosity_options_short();
				help_option();
				getopt_argument_missing();
				getopt_invalid_option();
				invalid_option(opt);
			}
		}
		if (optind < argc)
		{
			int			i = optind + argo;
			int			index = 0;

			char *		*arguments[] = {
				&serial,
				&maca,
				&wlanKey,
				&tr069Passphrase,
				NULL
			};

			while (argv[i])
			{
				if (!argv[i + 1])
				{
					if (isatty(0) && !tty)
					{
						if (checkLastArgumentIsInputFile(argv[i]))
							break;
					}
				}
				*(arguments[index++]) = argv[i++];
				if (!arguments[index])
				{
					if (argv[i])
					{
						if (checkLastArgumentIsInputFile(argv[i]))
							i++;
					}
					warnAboutExtraArguments(argv,i);
					break;
				}
			}
		}
	}

	if (isAnyError())
		return EXIT_FAILURE;

	resetError();

	CipherSizes();

	char				key[*cipher_keyLen];

	memset(key, 0, *cipher_keyLen);

	if (!serial) /* use device properties from running system */
	{
		altenv_verbose_message();

		if (!keyFromDevice(hash, &hashLen, false))
			return EXIT_FAILURE; /* error message was displayed from called function */

		memcpy(key, hash, *cipher_ivLen);
		hexBuffer = malloc((hashLen * 2) + 1);

		if (hexBuffer)
		{
			hexLen = binaryToHexadecimal((char *) hash, hashLen, hexBuffer, (hashLen * 2) + 1);
			*(hexBuffer + hexLen) = 0;
			verboseMessage(verboseDeviceKeyHash, hexBuffer);
			free(hexBuffer);
		}
	}
	else if (!maca) /* single argument - assume it's a hexadecimal key already */
	{
		if (altEnv)
		{
			warningMessage(verboseAltEnvIgnored);
			failOnStrict();
		}

		if (strlen(serial) != 32)
		{
			errorMessage(errorWrongHexKeyLength, serial);
			return EXIT_FAILURE;
		}

		hexadecimalToBinary(serial, strlen(serial), key, *cipher_keyLen);

		if (isAnyError())
		{
			errorMessage(errorInvalidKeyValue, serial);
			return EXIT_FAILURE;
		}

		verboseMessage(verboseUsingKey, serial);
	}
	else if (!wlanKey) /* serial and maca - use an export key from device */
	{
		if (altEnv)
		{
			warningMessage(verboseAltEnvIgnored);
			failOnStrict();
		}

		errorMessage(errorDeviceProperties, URLADER_SERIAL_NAME, URLADER_MACA_NAME, URLADER_WLANKEY_NAME);

		return EXIT_FAILURE;
	}
	else
	{
		if (altEnv)
		{
			warningMessage(verboseAltEnvIgnored);
			failOnStrict();
		}

		verboseMessage(verboseSerialUsed, serial);
		verboseMessage(verboseMACUsed, maca);
		verboseMessage(verboseWLANKeyUsed, wlanKey);
		if (tr069Passphrase)
			verboseMessage(verboseTR069PPUsed, tr069Passphrase);

		if (!keyFromProperties(hash, &hashLen, serial, maca, wlanKey, tr069Passphrase))
		{
			return EXIT_FAILURE;
		}

		memcpy(key, hash, *cipher_ivLen);
		hexBuffer = malloc((hashLen * 2) + 1);

		if (hexBuffer)
		{
			hexLen = binaryToHexadecimal((char *) hash, hashLen, hexBuffer, (hashLen * 2) + 1);
			*(hexBuffer + hexLen) = 0;
			verboseMessage(verboseUsingKey, hexBuffer);
			free(hexBuffer);
		}
	}

	if (isatty(0) && !tty)
	{
		errorMessage(errorReadFromTTY);
		return EXIT_FAILURE;
	}

	memoryBuffer_t		*inputFile = memoryBufferReadFile(stdin, -1);
	
	if (!inputFile)
	{
		if (!isAnyError()) /* empty input file */
			return EXIT_SUCCESS;	
		errorMessage(errorReadToMemory);
		return EXIT_FAILURE;
	}

	memoryBufferProcessFile(&inputFile, 0, key, stdout);

	inputFile = memoryBufferFreeChain(inputFile);

	return (isAnyError() ? EXIT_FAILURE : EXIT_SUCCESS);
}

#pragma GCC diagnostic pop
