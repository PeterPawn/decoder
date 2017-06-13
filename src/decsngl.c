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

#define DECSNGL_C

#include "common.h"
#include "decsngl_usage.c"

static	char *				__commandNames[] = {
#include "decsngl_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static	commandEntry_t 		__decsngl_command = { .names = &commandNames, .ep = &decsngl_entry, .usage = &decsngl_usage, .usesCrypto = true };
EXPORTED commandEntry_t *	decsngl_command = &__decsngl_command;

// 'decode_secret' function - decode the specified secret value (in Base32 encoding) perties

int		decsngl_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	bool				hexOutput = false;
	char *				out = NULL;
	size_t				outLen = 0;
	char *				secret = NULL;
	char *				key = NULL;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-output", no_argument, NULL, 'x' },
		};
		char *			options_short = "x" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexOutput = true;
					entry->finalNewlineOnTTY = true;
					break;

				check_verbosity_options_short();
				help_option();
				getopt_invalid_option();
				invalid_option(opt);
			}
		}
		if (optind >= (argc - argo))
		{
			errorMessage(errorPasswordMissing);
			return EXIT_FAILURE;
		}
		else
		{
			int			i = optind + argo;
			int			index = 0;

			char *		*arguments[] = {
				&secret,
				&key,
				NULL
			};

			while (argv[i])
			{
				*(arguments[index++]) = argv[i++];
				if (!arguments[index])
					break;
			}
			if (!key)
			{
				errorMessage(errorWrongArgumentsCount);
				__usage(false, false);
				return EXIT_FAILURE;
			}
		}
	}
	else
	{
		errorMessage(errorWrongArgumentsCount);
		__usage(false, false);
		return EXIT_FAILURE;
	}

	resetError();

	CipherSizes();

	size_t				secretBufSize = base32ToBinary(secret, (size_t) -1, NULL, 0);
	size_t				keyBufSize = hexadecimalToBinary(key, (size_t) -1, NULL, 0);
	size_t				secretSize = 0;
	size_t				decryptedSize = 0;
	size_t				keySize = 0;
	size_t				dataLen = 0;
	bool				isString = false;

	char *				secretBuffer = (char *) malloc(secretBufSize + *cipher_blockSize);
	char *				decryptedBuffer = (char *) malloc(secretBufSize + *cipher_blockSize);
	char *				keyBuffer = (char *) malloc(*cipher_keyLen);
	char *				hexBuffer = NULL;

	if (!secretBuffer || !decryptedBuffer || !keyBuffer)
	{
		errorMessage(errorNoMemory);
		return EXIT_FAILURE;
	}

	memset(secretBuffer, 0, secretBufSize + *cipher_blockSize);
	memset(decryptedBuffer, 0, secretBufSize + *cipher_blockSize);
	memset(keyBuffer, 0, *cipher_keyLen);
	
	resetError();
	secretSize = base32ToBinary(secret, (size_t) -1, (char *) secretBuffer, secretBufSize);
	keySize = hexadecimalToBinary(key, (size_t) -1, (char *) keyBuffer, keyBufSize);

	if (isAnyError())
	{
		errorMessage(errorInvalidArgumentData);
		secretBuffer = clearMemory(secretBuffer, secretBufSize + *cipher_blockSize, true);
		decryptedBuffer = clearMemory(decryptedBuffer, secretBufSize + *cipher_blockSize, true);
		keyBuffer = clearMemory(keyBuffer, dataLen * 2, true);
		return EXIT_FAILURE;
	}

	if (keySize != 16)
	{
		errorMessage(errorWrongKeySize);
		secretBuffer = clearMemory(secretBuffer, secretBufSize + *cipher_blockSize, true);
		decryptedBuffer = clearMemory(decryptedBuffer, secretBufSize + *cipher_blockSize, true);
		keyBuffer = clearMemory(keyBuffer, dataLen * 2, true);
		return EXIT_FAILURE;
	}

	CipherContext 		*ctx = CipherInit(NULL, CipherTypeValue, keyBuffer, secretBuffer, false);
	
	if (!(secretSize % 16))
		secretSize++;
	if (CipherUpdate(ctx, decryptedBuffer, &decryptedSize, secretBuffer + *cipher_ivLen, secretSize - *cipher_ivLen))
	{
		if (!DigestCheckValue(decryptedBuffer, decryptedSize, &out, &dataLen, &isString))
		{	
			setError(INVALID_KEY);
			errorMessage(errorWrongPassword);
		}
	}
	
	ctx = CipherCleanup(ctx);
	secretBuffer = clearMemory(secretBuffer, secretBufSize, true);
	keyBuffer = clearMemory(keyBuffer, keyBufSize, true);

	if (!isAnyError())
	{
		if (hexOutput)
		{
			hexBuffer = (char *) malloc((dataLen * 2) * 1);
			if (!hexBuffer)
			{
				errorMessage(errorNoMemory);
				setError(NO_MEMORY);
			}
			else
			{
				outLen = binaryToHexadecimal(out, dataLen, hexBuffer, (dataLen * 2) + 1);
				out = hexBuffer;
			}
		}
		else
			outLen = dataLen;
		if (!isAnyError() && fwrite(out, outLen, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			errorMessage(errorWriteFailed);
		}
	}

	hexBuffer = clearMemory(hexBuffer, *cipher_keyLen, true);
	decryptedBuffer = clearMemory(decryptedBuffer, secretBufSize + *cipher_blockSize, true);

	return EXIT_SUCCESS;
}
