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

static commandEntry_t 		__decfile_command = { .name = "decode_secrets", .ep = &decfile_entry, .usage = &decfile_usage, .usesCrypto = true };
EXPORTED commandEntry_t *	decfile_command = &__decfile_command;
#ifdef FREETZ_PACKAGE_DECRYPT_FRITZOS_CFG
static commandEntry_t 		__decfile_command = { .name = "decrypt-fritzos-cfg", .ep = &decfile_entry, .usage = &decfile_usage, .usesCrypto = true };
EXPORTED commandEntry_t *	decfile_command = &__decfile_command;
#endif

// statics

//// error messages ////
static	char *			errorWrongHexKeyLength = "The specified key value '%s' has a wrong length, 32 hexadecimal digits are the expected value.\n";
static	char *			errorInvalidKeyValue = "The specified key value '%s' is invalid, it contains wrong characters.\n";
static	char *			errorDeviceProperties = "To use the properties of another device, you have to specify at least three\nvalues ('%s', '%s' and '%s').\n";
static	char *			errorReadToMemory = "Error reading data into memory.\n";
//// end ////

// 'decode_secrets' function - decode all secret values from STDIN content and copy it with replaced values to STDOUT

int decfile_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	char 				hash[MAX_DIGEST_SIZE];
	size_t				hashLen = sizeof(hash);
	char *				serial = NULL;
	char *				maca = NULL;
	char *				wlanKey = NULL;
	char *				tr069Passphrase = NULL;
	bool				altEnv = false;
	bool				tty = false;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			{ "tty", no_argument, NULL, 't' },
			altenv_options_long,
			verbosity_options_long,
		};
		char *			options_short = ":" "t" altenv_options_short verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 't':
						tty = true;
						break;

				check_altenv_options_short();
				check_verbosity_options_short();
				help_option();
				getopt_message_displayed();
				getopt_argument_missing();
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

	resetError();

	altenv_verbose_message();

	CipherSizes();

	char				key[*cipher_keyLen];

	memset(key, 0, *cipher_keyLen);

	if (!serial) /* use device properties from running system */
	{
		if (!keyFromDevice(hash, &hashLen, false))
			return EXIT_FAILURE; /* error message was displayed from called function */
		memcpy(key, hash, *cipher_ivLen);
	}
	else if (!maca) /* single argument - assume it's a hexadecimal key already */
	{
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
	}
	else if (!wlanKey) /* serial and maca - use an export key from device */
	{
		errorMessage(errorDeviceProperties, URLADER_SERIAL_NAME, URLADER_MACA_NAME, URLADER_WLANKEY_NAME);
		return EXIT_FAILURE;
	}
	else
	{
		if (!keyFromProperties(hash, &hashLen, serial, maca, wlanKey, tr069Passphrase))
		{
			return EXIT_FAILURE;
		}
		memcpy(key, hash, *cipher_ivLen);
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
