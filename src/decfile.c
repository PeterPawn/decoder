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

static commandEntry_t 		__decfile_command = { .name = "decode_secrets", .ep = &decfile_entry, .usage = &decfile_usage };
EXPORTED commandEntry_t *	decfile_command = &__decfile_command;
#ifdef FREETZ_PACKAGE_DECRYPT_FRITZOS_CFG
static commandEntry_t 		__decfile_command = { .name = "decrypt-fritzos-cfg", .ep = &decfile_entry, .usage = &decfile_usage };
EXPORTED commandEntry_t *	decfile_command = &__decfile_command;
#endif

// display usage help

void 	decfile_usage(bool help)
{
	errorMessage("help for decode_secrets\n");
	if (help)
		errorMessage("option --help used\n");
}

// 'decode_secrets' function - decode all secret values from STDIN content and copy it with replaced values to STDOUT

int decfile_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	char 				hash[MAX_DIGEST_SIZE];
	size_t				hashLen = sizeof(hash);
	char *				serial = NULL;
	char *				maca = NULL;
	char *				wlanKey = NULL;
	char *				tr069Passphrase = NULL;

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
		if (optind < argc)
		{
			int		i = optind + argo;
			int		index = 0;

			char *	*arguments[] = {
				&serial,
				&maca,
				&wlanKey,
				&tr069Passphrase,
				NULL
			};

			while (argv[i])
			{
				*(arguments[index++]) = argv[i++];
				if (!arguments[index])
					break;
			}
		}
	}

	resetError();

	CipherSizes();

	char			key[*cipher_keyLen];

	memset(key, 0, *cipher_keyLen);

	if (!serial) /* use device properties from running system */
	{
		if (!keyFromDevice(hash, &hashLen, false))
			return EXIT_FAILURE;
		memcpy(key, hash, *cipher_ivLen);
	}
	else if (!maca) /* single argument - assume it's a hexadecimal key already */
	{
		hexadecimalToBinary(serial, strlen(serial), key, *cipher_keyLen);
		if (isAnyError())
			return EXIT_FAILURE;
	}
	else if (!wlanKey) /* serial and maca - use an export key from device */
	{
		errorMessage("To use the properties of another device, you have to specify at least three\nvalues ('%s', '%s' and '%s').\a\n", URLADER_SERIAL_NAME, URLADER_MACA_NAME, URLADER_WLANKEY_NAME);
		return EXIT_FAILURE;
	}
	else
	{
		if (!keyFromProperties(hash, &hashLen, serial, maca, wlanKey, tr069Passphrase))
			return EXIT_FAILURE;
		memcpy(key, hash, *cipher_ivLen);
	}

	memoryBuffer_t	*inputFile = memoryBufferReadFile(stdin, 8 * 1024);
	
	if (!inputFile)
	{
		if (!isAnyError()) /* empty input file */
			return EXIT_SUCCESS;	
		errorMessage("Error reading STDIN to memory.\a\n");
		return EXIT_FAILURE;
	}

	memoryBufferProcessFile(&inputFile, 0, key, stdout);

	inputFile = memoryBufferFreeChain(inputFile);

	return (isAnyError() ? EXIT_FAILURE : EXIT_SUCCESS);
}
