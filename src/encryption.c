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

#define ENCRYPTION_C

#include "common.h"

// FRITZ!OS specific crypto functions

// initialize static variables

EXPORTED	void	EncryptionInit(void)
{
	DigestSizes();
	CipherSizes();
}

// decrypt a Base32 value using the specified key

EXPORTED	bool	DecryptValue(CipherContext * ctx, char * cipherText, size_t valueSize, FILE * out, char * outBuffer, char * key, bool escaped)
{
	size_t			cipherBufSize = base32ToBinary(cipherText, valueSize, NULL, 0);
	size_t			cipherSize;
	char *			cipherBuffer = (char *) malloc(cipherBufSize + *cipher_blockSize);
	size_t			decryptedSize = 0;
	char *			decryptedBuffer = (char *) malloc(cipherBufSize + *cipher_blockSize);
	CipherContext 	*localCtx;

	cipherSize = base32ToBinary(cipherText, (size_t) -1, (char *) cipherBuffer, cipherBufSize + *cipher_blockSize);
	
	localCtx = (ctx ? ctx : EVP_CIPHER_CTX_new());
	CipherInit(localCtx, CipherTypeValue, key, cipherBuffer, false);
	verboseMessage(verboseFoundCipherText, cipherText);
	if (!(cipherSize % *cipher_blockSize))
		cipherSize++;
	if (CipherUpdate(localCtx, decryptedBuffer, &decryptedSize, cipherBuffer + *cipher_ivLen, cipherSize - *cipher_ivLen))
	{
		char *		value;
		size_t		valueSize = 0;
		bool		isString = false;

		if (DigestCheckValue(decryptedBuffer, decryptedSize, &value, &valueSize, &isString))
		{
			if (out && valueSize && escaped)
			{
				int	start = 0;

				verboseMessage(verboseDecryptedTo, value);
				for (int i = 0; i < (int) valueSize; i++)
				{
					if (*(value + i) == '\\' || *(value + i) == '"') /* split output */
					{	
						if ((i > start) && fwrite((value + start), (i - start), 1, out) != 1)
						{
							setError(WRITE_FAILED);
							break;
						}
						if (fwrite("\\", 1, 1, out) != 1)
						{
							setError(WRITE_FAILED);
							break;
						}
						start = i;
					}
				}
				valueSize -= start;
				value += start;
			}
			if (out && (valueSize > 0) && (fwrite(value, valueSize, 1, out) != 1))
				setError(WRITE_FAILED);
			if (!out && outBuffer)
			{
				memcpy(outBuffer, value, valueSize + (isString ? 1 : 0));	
			}
			if (!isString)
			{
				char *	hexBuffer = malloc((valueSize * 2) + 1);

				if (hexBuffer)
				{
					binaryToHexadecimal(value, valueSize, hexBuffer, (valueSize * 2) + 1);
					*(hexBuffer + (valueSize * 2)) = 0;
					verboseMessage(verboseDecryptedToHex, hexBuffer);
					free(hexBuffer);
				}
				else
					verboseMessage(verboseDisplayFailed);
			}
		}
		else
		{
			setError(DECRYPT_ERR);
			verboseMessage(verboseDecryptFailed);
		}
	}
	cipherBuffer = clearMemory(cipherBuffer, cipherBufSize, true);
	decryptedBuffer = clearMemory(decryptedBuffer, cipherBufSize + *cipher_blockSize, true);
	if (!ctx)
		localCtx = CipherCleanup(localCtx);

	return !isAnyError();
}

// re-compute and compare the cleartext digest for the specified buffer

EXPORTED	bool	DigestCheckValue(char *buffer, size_t bufferSize, char * *value, size_t * dataLen, bool * string)
{
	char			hash[MAX_DIGEST_SIZE];
	size_t			hashLen = sizeof(hash);

	if ((hashLen = Digest(buffer + 4, bufferSize - 4, hash, hashLen)) == 0)
		return false;

	if (memcmp(buffer, hash, 4))
		return false;

	*dataLen = 	(*((unsigned char *) buffer + 4) << 24) +
				(*((unsigned char *) buffer + 5) << 16) + 
				(*((unsigned char *) buffer + 6) << 8) + 
				(*((unsigned char *) buffer + 7));
	
	*value = buffer + 8;
	if (*(buffer + 8 + *dataLen - 1) == 0)
	{
		(*dataLen)--;
		*string = true;
	}
	
	return dataLen;
}

// decrypt a binary encrypted file (CRYPTEDBINFILE)

EXPORTED	bool	DecryptFile(char * input, size_t inputSize, FILE * out, char * outBuffer, char * key)
{
	char *				decryptedBuffer = malloc(inputSize + *cipher_blockSize);
	CipherContext *		ctx = EVP_CIPHER_CTX_new();
	char *				current = decryptedBuffer;
	size_t				outSize = 0;
	size_t				ivLen = *cipher_ivLen;
	char * 				iv = (char *) malloc(ivLen);
	size_t				dataSize = 0;
	size_t				offset = 0;

	if (!ctx)
		return false;

	if (!decryptedBuffer)
	{
		setError(NO_MEMORY);
		return false;
	}

	memset(iv, 0, ivLen);
	CipherInit(ctx, CipherTypeFile, key, iv, false);
	outSize = inputSize + *cipher_blockSize;
	CipherUpdate(ctx, decryptedBuffer, &outSize, input, inputSize + *cipher_blockSize);
	dataSize += outSize;
	CipherFinal(ctx, decryptedBuffer, &outSize);
	dataSize += outSize;
	CipherCleanup(ctx);
	offset = (isError(OSSL_CIPHER_ERR) ? 0 : *cipher_keyLen);
	resetError();

	current = decryptedBuffer + dataSize - *cipher_blockSize - offset;
	if (*(current) == 'A' && *(current + 1) == 'V' && *(current + 2) == 'M')
	{
		current += 3;
		for (int i = 0; i < 12 - 3; i++)
		{
			if (*(current + i) != 0)
			{
				setError(DECRYPT_ERR);
				break;
			}
		}
	}
	else
		setError(DECRYPT_ERR);

	if (!isAnyError())
	{
		current = decryptedBuffer + dataSize - *cipher_blockSize - offset + 12;

		size_t	dataSize =	(*((unsigned char *) current) << 24) +
							(*((unsigned char *) current + 1) << 16) +
							(*((unsigned char *) current + 2) << 8) +
							(*((unsigned char *) current + 3));

		if (dataSize)
		{
			if (out && (fwrite(decryptedBuffer + 4, dataSize, 1, out) != 1))
				setError(WRITE_FAILED);
			if (outBuffer)
				memcpy(outBuffer, decryptedBuffer + 4, dataSize);
		}
	}

	clearMemory(decryptedBuffer, inputSize, true);
	clearMemory(iv, ivLen, true);

	return !isAnyError();
}

EXPORTED	bool	keyFromDevice(char * hash, size_t * hashSize, bool forExport)
{
	memoryBuffer_t *	env = getEnvironmentFile();

	if (!env)
		return false;

	DigestContext	 	*ctx = DigestInit();

	struct variables {
		char *			name;
		char *			show;
		char *			append;
		bool			errorIfMissing;
		bool			export;
	}					envVariables[] = {
						{ .name = URLADER_SERIAL_NAME, .show = URLADER_SERIAL_NAME, .append = "\n", .errorIfMissing = true, .export = true },
						{ .name = URLADER_MACA_NAME, .show = URLADER_MACA_NAME, .append = "\n", .errorIfMissing = true, .export = true },
						{ .name = URLADER_WLANKEY_NAME, .show = URLADER_WLANKEY_NAME, .append = NULL, .errorIfMissing = true, .export = false },
						{ .name = URLADER_TR069PP_NAME, .show = URLADER_TR069PP_NAME, .append = NULL, .errorIfMissing = false, .export = false },
						{ .name = NULL, .show = NULL, .append = NULL, .errorIfMissing = false },
	};
	struct variables 	*var = envVariables;

	while (var && var->name)
	{
		char *			value = getEnvironmentValue(env, var->name);

		if (value)
		{
			verboseMessage(verboseFoundProperty, var->show, value);
			DigestUpdate(ctx, value, strlen(value));
			if (var->append)
				DigestUpdate(ctx, var->append, strlen(var->append));
		}
		else
		{
			if (var->errorIfMissing == true)
			{
				errorMessage(errorMissingDeviceProperty, var->show);
				setError(URLADER_ENV_ERR);
				break;
			}
			verboseMessage(verboseMissingProperty, var->show);
		}
		var++;
		if (forExport && !var->export)
			break;
	}

	env = memoryBufferFreeChain(env);

	*hashSize = *digest_blockSize;
	DigestFinal(ctx, hash);
	ctx = DigestCleanup(ctx);

	return !isAnyError();
}

// check MAC address format

bool	checkMACAddress(char * mac)
{
	int		i = 0;
	int		j = 0;
	char *	curr = mac;

	while (*curr != 0)
	{
		if (j == 2)
		{
			if (*curr != ':')
				return false;
			else
			{
				j = 0;
				i += 1;
			}
		}
		else
		{
			if ((*curr < '0' || *curr > '9') && (*curr < 'A' || *curr > 'F'))
				return false;
			j += 1;
		}
		curr++;
	}
	if (i == 5 && j == 2) return true;

	return false;
}

// password generation from properties

EXPORTED	bool	keyFromProperties(char * hash, size_t * hashSize, char * serial, char * maca, char * wlanKey, char * tr069Passphrase)
{
	DigestContext	 	*ctx = DigestInit();

	if (strlen(serial) != 16)
		verboseMessage(verboseWrongSerialLength, serial);
	DigestUpdate(ctx, serial, strlen(serial));
	DigestUpdate(ctx, "\n", 1);
	if (!checkMACAddress(maca))
		verboseMessage(verboseWrongMACAddress, maca);
	DigestUpdate(ctx, maca, strlen(maca));
	DigestUpdate(ctx, "\n", 1);
	if (wlanKey && *wlanKey)
	{
		if (strlen(wlanKey) != 16 && strlen(wlanKey) != 20)
			verboseMessage(verboseWrongWLANKey, wlanKey);
		DigestUpdate(ctx, wlanKey, strlen(wlanKey));
	}
	if (tr069Passphrase && *tr069Passphrase)
	{
		/* warn about unusual/unexpected length of data - I've seen 8 or 12 characters here
		if (strlen(tr069Passphrase) != 8 && strlen(tr069Passphrase != 12)
			verboseMessage(verboseWrongTR069Passphrase, tr069Passphrase);
		*/
		DigestUpdate(ctx, tr069Passphrase, strlen(tr069Passphrase));
	}
	*hashSize = *digest_blockSize;
	DigestFinal(ctx, hash);
	ctx = DigestCleanup(ctx);

	return !isAnyError();
}
