/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
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

#define DECSNGL_C

#include "common.h"
#include "decsngl_usage.c"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wformat-security"

static	char *				__commandNames[] = {
#include "decsngl_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static	commandEntry_t 		__decsngl_command = { .names = &commandNames, .ep = &decsngl_entry, .usage = &decsngl_usage, .short_desc = &decsngl_shortdesc, .usesCrypto = true, .finalNewlineOnTTY = true };
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
			options_long_end,
		};
		char *			options_short = ":" "x" verbosity_options_short;

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
			errorMessage(errorMissingArguments);
			__autoUsage();
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
				__autoUsage();
				return EXIT_FAILURE;
			}
			if (argv[i])
			{
				warnAboutExtraArguments(argv, i);
			}
		}
	}
	else
	{
		errorMessage(errorWrongArgumentsCount);
		__autoUsage();
		return EXIT_FAILURE;
	}

	if (isAnyError())
		return EXIT_FAILURE;

	resetError();

	CipherSizes();

	size_t				secretBufSize = base32ToBinary(secret, (size_t) -1, NULL, 0);
	size_t				keyBufSize = hexadecimalToBinary(key, (size_t) -1, NULL, 0);
	size_t				secretSize = 0;
	size_t				decryptedSize = 0;
	size_t				keySize = 0;
	size_t				dataLen = 0;
	bool				isString = false;
	bool				hexForced = false;

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

	if (keySize != 16 && keySize != 32)
	{
		errorMessage(errorWrongKeySize);
		secretBuffer = clearMemory(secretBuffer, secretBufSize + *cipher_blockSize, true);
		decryptedBuffer = clearMemory(decryptedBuffer, secretBufSize + *cipher_blockSize, true);
		keyBuffer = clearMemory(keyBuffer, dataLen * 2, true);
		return EXIT_FAILURE;
	}

	if (isVerbose())
	{
		hexBuffer = malloc((32 * 2) + 1);

		if (hexBuffer)
		{
			binaryToHexadecimal(keyBuffer, keySize, hexBuffer, (keySize * 2) + 1);
			*(hexBuffer + (keySize * 2)) = 0;
			verboseMessage(verboseDebugKey, keySize, hexBuffer);
			free(hexBuffer);
			hexBuffer = NULL;
		}

		verboseMessage(verboseDebugBase32, strlen(secret), secret);

		hexBuffer = malloc((secretSize * 2) + 1);

		if (hexBuffer)
		{
			binaryToHexadecimal(secretBuffer, secretSize, hexBuffer, (secretSize * 2) + 1);
			*(hexBuffer + (secretSize * 2)) = 0;
			verboseMessage(verboseDebugInput, secretSize, hexBuffer);
			free(hexBuffer);
			hexBuffer = NULL;
		}

		hexBuffer = malloc((*cipher_ivLen * 2) + 1);

		if (hexBuffer)
		{
			binaryToHexadecimal(secretBuffer, *cipher_ivLen, hexBuffer, (*cipher_ivLen * 2) + 1);
			*(hexBuffer + (*cipher_ivLen * 2)) = 0;
			verboseMessage(verboseDebugIV, *cipher_ivLen, hexBuffer);
			free(hexBuffer);
			hexBuffer = NULL;
		}

		hexBuffer = malloc(((secretSize - *cipher_ivLen) * 2) + 1);

		if (hexBuffer)
		{
			binaryToHexadecimal(secretBuffer + *cipher_ivLen, (secretSize - *cipher_ivLen), hexBuffer, ((secretSize - *cipher_ivLen) * 2) + 1);
			*(hexBuffer + ((secretSize - *cipher_ivLen) * 2)) = 0;
			verboseMessage(verboseDebugEncData, (secretSize - *cipher_ivLen), hexBuffer);
			free(hexBuffer);
			hexBuffer = NULL;
		}
	}

	CipherContext 		*ctx = CipherInit(NULL, CipherTypeValue, keyBuffer, secretBuffer, false);
	
	if (!(secretSize % 16))
		secretSize++;

	if (CipherUpdate(ctx, decryptedBuffer, &decryptedSize, secretBuffer + *cipher_ivLen, secretSize - *cipher_ivLen))
	{
		if (isVerbose())
		{
			hexBuffer = malloc((decryptedSize * 2) + 1);

			if (hexBuffer)
			{
				binaryToHexadecimal(decryptedBuffer, decryptedSize, hexBuffer, (decryptedSize * 2) + 1);
				*(hexBuffer + (decryptedSize * 2)) = 0;
				verboseMessage(verboseDebugDecData, decryptedSize, hexBuffer);
				free(hexBuffer);
				hexBuffer = NULL;
			}
		}
		if (!digestCheckValue(decryptedBuffer, decryptedSize, &out, &dataLen, &isString))
		{	
			setError(INVALID_KEY);
			errorMessage(errorWrongPassword);
		}
		else
		{
			if (isVerbose())
			{
				verboseMessage(verboseDebugSize, dataLen + (isString ? 1 : 0));
				verboseMessage(verboseDebugIsString, (isString && !hexOutput ? "yes" : "no"));
			}
		}
	}
	
	ctx = CipherCleanup(ctx);
	secretBuffer = clearMemory(secretBuffer, secretBufSize, true);
	keyBuffer = clearMemory(keyBuffer, keyBufSize, true);

	if (!isAnyError())
	{
		if (!hexOutput && !isString)
			hexForced = true;

		if (hexOutput || hexForced)
		{
			hexBuffer = (char *) malloc((dataLen * 2) + 5);
			if (!hexBuffer)
			{
				errorMessage(errorNoMemory);
				setError(NO_MEMORY);
			}
			else
			{
				char *	target = hexBuffer;

				if (hexForced || hexOutput)
				{
					strcpy(target, "0x");
					target += 2;
				}

				if (isString)
					dataLen += 1;

				outLen = binaryToHexadecimal(out, dataLen, target, (dataLen * 2) + 1);
				outLen += (hexForced ? 2 : 0);
				*(target + (dataLen * 2)) = 0;
				out = hexBuffer + (hexOutput ? 2 : 0);
				verboseMessage(verboseDebugValue, dataLen, hexBuffer);
			}
		}
		else
		{
			outLen = dataLen;
			verboseMessage(verboseDebugValue, strlen(out), out);
		}

		if (!isAnyError() && fwrite(out, outLen, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			errorMessage(errorWriteFailed);
		}
	}

	hexBuffer = clearMemory(hexBuffer, 1, true);
	decryptedBuffer = clearMemory(decryptedBuffer, secretBufSize + *cipher_blockSize, true);

	return EXIT_SUCCESS;
}

#pragma GCC diagnostic pop
