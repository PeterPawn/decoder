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

#define DECEXP_C

#include "common.h"
#include "decexp_usage.c"

static	char *				__commandNames[] = {
#include "decexp_commands.c"
		NULL
};
static	char * *			commandNames = &__commandNames[0];
static	commandEntry_t 		__decexp_command = { .names = &commandNames, .ep = &decexp_entry, .usage = &decexp_usage, .usesCrypto = true };
EXPORTED commandEntry_t *	decexp_command = &__decexp_command;

// 'decode_export' function - decode all secret values from the export file on STDIN and copy it with replaced values to STDOUT

int		decexp_entry(int argc, char** argv, int argo, commandEntry_t * entry)
{
	char 				hash[MAX_DIGEST_SIZE];
	size_t				hashLen = sizeof(hash);
	char *				serial = NULL;
	char *				maca = NULL;
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
			options_long_end,
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

	memoryBuffer_t		*inputFile = memoryBufferReadFile(stdin, -1);
	
	if (!inputFile)
	{
		if (!isAnyError()) /* empty input file */
			return EXIT_SUCCESS;	
		errorMessage(errorReadToMemory);
		return EXIT_FAILURE;
	}

	memoryBuffer_t *	current = inputFile;
	memoryBuffer_t *	found = current;
	size_t				offset = 0;
	size_t				foundOffset = offset;
	size_t				valueSize = 0;
	char *				varName;
	bool				split = false;

	if ((varName = memoryBufferFindString(&found, &foundOffset, EXPORT_PASSWORD_NAME, strlen(EXPORT_PASSWORD_NAME) , &split)) != NULL)
	{
		current = found;
		offset = foundOffset;

		memoryBufferAdvancePointer(&current, &offset, strlen(EXPORT_PASSWORD_NAME));
		found = current;
		foundOffset = offset;
		memoryBufferSearchValueEnd(&found, &foundOffset, &valueSize, &split);

		if (valueSize != 104)
		{
			errorMessage(errorInvalidFirstStageLength, (uint32_t) valueSize, EXPORT_PASSWORD_NAME);
			setError(INV_DATA_SIZE);
			inputFile = memoryBufferFreeChain(inputFile);
			return EXIT_FAILURE;
		}

		char *			copy;
		char *			cipherText = (char *) malloc(valueSize + 1);
		bool			passwordIsCorrect = false;
			
		memset(cipherText, 0, valueSize + 1);
		copy = cipherText;
		while (current && (current != found))
		{
			memcpy(copy, current->data + offset, current->used - offset);
			copy += (current->used - offset);
			current = current->next;
			offset = 0;
		}
		memcpy(copy, current->data + offset, foundOffset - offset);
		passwordIsCorrect = DecryptValue(NULL, cipherText, valueSize, NULL, key, key, false);
		memset(key + *cipher_ivLen, 0, *cipher_keyLen - *cipher_ivLen);
		if (passwordIsCorrect)
		{
			char 		hex[(MAX_DIGEST_SIZE * 2) + 1];
			size_t		hexLen = binaryToHexadecimal(key, *cipher_keyLen - *cipher_ivLen, hex, (MAX_DIGEST_SIZE * 2) + 1);

			hex[hexLen] = 0;
			verboseMessage(verboseUsingKey, hex);

			cipherText = clearMemory(cipherText, valueSize + 1, true);
			current = inputFile;
			offset = 0;
			while (current && (current != found)) /* output data in front of password field */
			{
				if (fwrite(current->data + offset, current->used - offset, 1, stdout) != 1)
				{
					setError(WRITE_FAILED);
					break;
				}
				current = current->next;
				offset = 0;
			}
			if (current)
			{
				if (fwrite(current->data + offset, foundOffset - offset, 1, stdout) != 1)
					setError(WRITE_FAILED);
				else
					offset = foundOffset;
			}
		}
		else
		{
			setError(DECRYPT_ERR);
			errorMessage(errorDecryptionFailed);
		}
	}
	else
	{
		errorMessage(errorNoPasswordEntry);
		setError(INVALID_FILE);
	}

	if (!isAnyError())
		memoryBufferProcessFile(&found, foundOffset, key, stdout);

	inputFile = memoryBufferFreeChain(inputFile);

	return (isAnyError() ? EXIT_FAILURE : EXIT_SUCCESS);
}
