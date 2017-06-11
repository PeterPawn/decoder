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

#define DECCB_C

#include "common.h"
#include "deccb_usage.c"

static commandEntry_t 		__deccb_command = { .name = &commandNames, .ep = &deccb_entry, .usage = &deccb_usage, .usesCrypto = true };
EXPORTED commandEntry_t *	deccb_command = &__deccb_command;
static	char *				commandNames = {
#include "deccb_commands.c"
		NULL
};

// statics

//// error messages ////
static	char *			errorDecryptFileData = "Unable to decrypt file data.\n";
static	char *			errorReadToMemory = "Error reading data into memory.\n";
static	char *			errorNoMemory = "Memory allocation error.\n";
//// end ////

// 'decode_crypedbinfile' function - decode the content of an encrypted binary file body from STDIN and copy the result to STDOUT

int		deccb_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	char 				hash[MAX_DIGEST_SIZE];
	size_t				hashLen = sizeof(hash);
	char *				serial = NULL;
	char *				maca = NULL;

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
					break;
			}
		}
	}

	resetError();

	CipherSizes();

	char				key[*cipher_keyLen];

	memset(key, 0, *cipher_keyLen);

	if (!serial) /* use device properties from running system */
	{
		if (!keyFromDevice(hash, &hashLen, true))
			return EXIT_FAILURE;
		memcpy(key, hash, *cipher_ivLen);
	}
	else if (!maca) /* single argument - assume it's a user-defined password */
	{
		hashLen = Digest(serial, strlen(serial), hash, hashLen);
		if (isAnyError())
			return EXIT_FAILURE;
		memcpy(key, hash, *cipher_ivLen);
	}
	else
	{
		if (!keyFromProperties(hash, &hashLen, serial, maca, NULL, NULL))
			return EXIT_FAILURE;
		memcpy(key, hash, *cipher_ivLen);
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
		}
	}

	size_t				hexSize = memoryBufferDataSize(inputFile);
	size_t				binSize = 0;
	char *				binBuffer = (char *) malloc(hexSize / 2);
	
	if (!binBuffer)
	{
		setError(NO_MEMORY);
		errorMessage(errorNoMemory);
		inputFile = memoryBufferFreeChain(inputFile);
		return EXIT_FAILURE;
	}

	binSize = hexSize / 2;
	binSize = hexadecimalToBinary(inputFile->data, inputFile->used, binBuffer, binSize);

	if (!isAnyError())
	{
		DecryptFile(binBuffer, binSize, stdout, NULL, key);
		if (isError(DECRYPT_ERR))
			errorMessage(errorDecryptFileData);
	}

	binBuffer = clearMemory(binBuffer, (hexSize / 2), true);
	inputFile = memoryBufferFreeChain(inputFile);

	return (isAnyError() ? EXIT_FAILURE : EXIT_SUCCESS);
}
