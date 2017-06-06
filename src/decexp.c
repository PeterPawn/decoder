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

static commandEntry_t 		__decexp_command = { .name = "decode_export", .ep = &decexp_entry, .usage = &decexp_usage };
EXPORTED commandEntry_t *	decexp_command = &__decexp_command;

// display usage help

void 	decexp_usage(bool help)
{
	errorMessage("help for decode_export\n");
	if (help)
		errorMessage("option --help used\n");
}

// 'decode_export' function - decode all secret values from the export file on STDIN and copy it with replaced values to STDOUT

int		decexp_entry(int argc, char** argv, int argo, commandEntry_t * entry)
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
			int		i = optind + argo;
			int		index = 0;

			char *	*arguments[] = {
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

	char			key[*cipher_keyLen];

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

	memoryBuffer_t	*inputFile = memoryBufferReadFile(stdin, 8 * 1024);
	
	if (!inputFile)
	{
		if (!isAnyError()) /* empty input file */
			return EXIT_SUCCESS;	
		errorMessage("Error reading STDIN to memory.\a\n");
		return EXIT_FAILURE;
	}

	memoryBuffer_t *	current = inputFile;
	memoryBuffer_t *	found = current;
	size_t				offset = 0;
	size_t				foundOffset = offset;
	size_t				valueSize = 0;
	char *				name;
	bool				split = false;

	if ((name = memoryBufferFindString(&found, &foundOffset, EXPORT_PASSWORD_NAME, strlen(EXPORT_PASSWORD_NAME) , &split)) != NULL)
	{
		current = found;
		offset = foundOffset;

		memoryBufferAdvancePointer(&current, &offset, strlen(EXPORT_PASSWORD_NAME));
		found = current;
		foundOffset = offset;
		memoryBufferSearchValueEnd(&found, &foundOffset, &valueSize, &split);

		if (valueSize > 80)
		{
			errorMessage("Invalid length of data (%u) in the '%s' entry. Expected value is 80.\a\n", (uint32_t) valueSize, EXPORT_PASSWORD_NAME);
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
			verboseMessage("using key 0x%s for decryption\n", hex);

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
			errorMessage("Decryption failed with the specified arguments.\a\n");	
		}
	}
	else
	{
		errorMessage("Unable to find the password entry in the provided file.\nIs this really an export file?\a\n");
		setError(INVALID_FILE);
	}

	if (!isAnyError())
		memoryBufferProcessFile(&found, foundOffset, key, stdout);

	inputFile = memoryBufferFreeChain(inputFile);

	return (isAnyError() ? EXIT_FAILURE : EXIT_SUCCESS);
}
