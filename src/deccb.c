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

#define DECCB_C

#include "common.h"
#include "deccb_usage.c"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wformat-security"

static	char *				__commandNames[] = {
#include "deccb_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static	commandEntry_t 		__deccb_command = { .names = &commandNames, .ep = &deccb_entry, .usage = &deccb_usage, .short_desc = &deccb_shortdesc, .usesCrypto = true };
EXPORTED commandEntry_t *	deccb_command = &__deccb_command;

// 'decode_crypedbinfile' function - decode the content of an encrypted binary file body from STDIN and copy the result to STDOUT

int		deccb_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	char 				hash[MAX_DIGEST_SIZE];
	size_t				hashLen = sizeof(hash);
	char *				serial = NULL;
	char *				maca = NULL;
	bool				tty = false;
	bool				binInput = false;
	bool				hexOutput = false;
	bool				altEnv = false;
	char *				hexBuffer = NULL;
	size_t				hexLen = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			{ "tty", no_argument, NULL, 't' },
			{ "hex-output", no_argument, NULL, 'x' },
			{ "bin-input", no_argument, NULL, 'b' },
			width_options_long,
			altenv_options_long,
			verbosity_options_long,
			options_long_end,
		};
		char *			options_short = ":" "txb" width_options_short altenv_options_short verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 't':
					tty = true;
					break;

				case 'x':
					hexOutput = true;
					entry->finalNewlineOnTTY = true;
					break;

				case 'b':
					binInput = true;
					break;

				check_altenv_options_short();
				check_width_options_short();
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
				NULL
			};

			while (argv[i])
			{
				*(arguments[index++]) = argv[i++];
				if (!arguments[index])
				{
					warnAboutExtraArguments(argv, i);
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

		if (!keyFromDevice(hash, &hashLen, true))
			return EXIT_FAILURE;

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
	else if (!maca) /* single argument - assume it's a user-defined password */
	{
		if (altEnv)
		{
			warningMessage(verboseAltEnvIgnored);
			failOnStrict();
		}

		verboseMessage(verbosePasswordUsed, serial);
		hashLen = Digest(serial, strlen(serial), hash, hashLen);

		if (isAnyError())
			return EXIT_FAILURE;

		memcpy(key, hash, *cipher_ivLen);
		hexBuffer = malloc((hashLen * 2) + 1);
		if (hexBuffer)
		{
			hexLen = binaryToHexadecimal((char *) hash, hashLen, hexBuffer, (hashLen * 2) + 1);
			*(hexBuffer + hexLen) = 0;
			verboseMessage(verbosePasswordHash, hexBuffer);
			free(hexBuffer);
		}
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

		if (!keyFromProperties(hash, &hashLen, serial, maca, NULL, NULL))
			return EXIT_FAILURE;

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

	if (getLineWrap() && !hexOutput)
	{
		warningMessage(verboseWrapLinesIgnored);
		failOnStrict();
	}

	if (isatty(0) && !tty)
	{
		errorMessage(errorReadFromTTY);
		return EXIT_FAILURE;
	}

	memoryBuffer_t	*inputFile = memoryBufferReadFile(stdin, -1);
	
	if (!inputFile)
	{
		if (!isAnyError()) /* empty input file */
			return EXIT_SUCCESS;	
		errorMessage(errorReadToMemory);
		return EXIT_FAILURE;
	}

	if (inputFile->next)
	{
		memoryBuffer_t	*consolidated = memoryBufferConsolidateData(inputFile);
		if (!consolidated)
		{
			errorMessage(errorNoMemory);
			inputFile = memoryBufferFreeChain(inputFile);
			return EXIT_FAILURE;
		}
		else
		{
			inputFile = memoryBufferFreeChain(inputFile);
			inputFile = consolidated;
			verboseMessage(verboseInputDataConsolidated, memoryBufferDataSize(inputFile));
		}
	}

	size_t				hexSize = memoryBufferDataSize(inputFile);
	size_t				binSize = 0;
	char *				binBuffer = NULL;
	char *				buffer;

	if (binInput)
	{
		binSize = hexSize;
		buffer = inputFile->data;
	}
	else
	{
		binBuffer = (char *) malloc(hexSize / 2);
	
		if (!binBuffer)
		{
			setError(NO_MEMORY);
			errorMessage(errorNoMemory);
			inputFile = memoryBufferFreeChain(inputFile);
			return EXIT_FAILURE;
		}

		binSize = hexSize / 2;
		binSize = hexadecimalToBinary(inputFile->data, inputFile->used, binBuffer, binSize);
		buffer = binBuffer;
		if (binSize == 0)
		{
			errorMessage(errorInvalidHexValue);
			setError(INV_HEX_DATA);
		}
	}

	if (!isAnyError())
	{
		decryptFile(buffer, binSize, stdout, NULL, key, hexOutput);
		if (isError(DECRYPT_ERR))
			errorMessage(errorDecryptFileData);
	}

	binBuffer = clearMemory(binBuffer, (hexSize / 2), true);
	inputFile = memoryBufferFreeChain(inputFile);

	return (isAnyError() ? EXIT_FAILURE : EXIT_SUCCESS);
}

#pragma GCC diagnostic pop
