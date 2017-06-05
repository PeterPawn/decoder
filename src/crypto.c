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

#define CRYPTO_C

#include "common.h"

// static variables

static 		size_t	__cipher_keyLen = -1;
static		size_t	__cipher_ivLen = -1;
static		size_t	__cipher_blockSize = -1;
static 		size_t	__digest_blockSize = -1;
EXPORTED	size_t	*cipher_keyLen = &__cipher_keyLen;
EXPORTED	size_t	*cipher_ivLen = &__cipher_ivLen;
EXPORTED	size_t	*cipher_blockSize = &__cipher_blockSize;
EXPORTED	size_t	*digest_blockSize = &__digest_blockSize;

// cipher functions

// various size settings 

EXPORTED	void	CipherSizes()
{
	*cipher_keyLen = EVP_CIPHER_key_length(CipherType);
	*cipher_ivLen = EVP_CIPHER_iv_length(CipherType);
	*cipher_blockSize = EVP_CIPHER_block_size(CipherType);
}

// initialize a cipher context

EXPORTED	CipherContext *	CipherInit(CipherContext * ctx, char * key, char * iv)
{
	CipherContext	*cipherCTX = NULL;

	if (ctx)
	{
		if (key && iv) /* reset context first */
		{
			cipherCTX = ctx;
			EVP_CIPHER_CTX_init(cipherCTX);
		}
	}
	else
	{
		cipherCTX = EVP_CIPHER_CTX_new();
		if (!cipherCTX)
			returnError(OSSL_CIPHER_ERR, NULL);
	}
	if (!key && !iv)
		return cipherCTX;
	if (EVP_DecryptInit_ex(cipherCTX, CipherType, NULL, (unsigned char *) key, (unsigned char *) iv))
		return cipherCTX;
	returnError(OSSL_CIPHER_ERR, NULL);
}

// cleanup a context and free the used memory

EXPORTED 	CipherContext *	CipherCleanup(CipherContext * ctx)
{	
	if (!ctx)
		return NULL;
	EVP_CIPHER_CTX_cleanup(ctx);
	EVP_CIPHER_CTX_free(ctx);
	return NULL;
}

// update a CBC context

EXPORTED	bool	CipherUpdate(CipherContext * ctx, char *output, size_t *outputSize, char *input, size_t inputSize)
{
	if (!ctx)
		return false;
	if (!EVP_DecryptUpdate(ctx, (unsigned char *) output, (int *) outputSize, (unsigned char *) input, inputSize))
	{
		setError(OSSL_CIPHER_ERR);
		return false;
	}
	return true;
}

// digest functions

// get digest value length

EXPORTED	void	DigestSizes()
{
	*digest_blockSize = EVP_MD_size(DigestType);
}

// initialize a digest context

EXPORTED	DigestContext * DigestInit()
{
	DigestContext	*ctx = EVP_MD_CTX_create();

	if (!ctx)
		setError(OSSL_DIGEST_ERR);
	else
	{
		if (!EVP_DigestInit_ex(ctx, DigestType, NULL))
		{
			setError(OSSL_DIGEST_ERR);
			EVP_MD_CTX_destroy(ctx);
			ctx = NULL;
		}
	}
	return ctx;
}

// update a digest context from the specified buffer

EXPORTED	bool	DigestUpdate(DigestContext * ctx, char * buffer, size_t bufferSize)
{
	if (!ctx)
		return false;
	if (!EVP_DigestUpdate(ctx, buffer, bufferSize))
	{
		setError(OSSL_DIGEST_ERR);
		return false;
	}
	return true;
}

// finalize a digest value

EXPORTED	bool	DigestFinal(DigestContext * ctx, char * output)
{
	if (!ctx)
		return false;
	if (!EVP_DigestFinal_ex(ctx, (unsigned char *) output, NULL))
	{
		setError(OSSL_DIGEST_ERR);
		return false;
	}
	return true;
}

// cleanup a digest context, free resources

EXPORTED	DigestContext *	DigestCleanup(DigestContext * ctx)
{
	if (!ctx)
		return NULL;
	EVP_MD_CTX_destroy(ctx);
	return NULL;
}

// compute and return a digest value for the specified buffer, all context handling
// is encapsulated

EXPORTED	size_t	Digest(char *buffer, size_t bufferSize, char *digest, size_t digestSize)
{
	resetError();

	DigestContext	*ctx = DigestInit();

	if (isAnyError())
		return false;

	if (digestSize < *digest_blockSize)
		setError(BUF_TOO_SMALL);
	else
	{
		if (DigestUpdate(ctx, buffer, bufferSize))
		{
			DigestFinal(ctx, digest);
		}		
	}
	ctx = DigestCleanup(ctx);

	return (isAnyError() ? 0 : *digest_blockSize);
}
