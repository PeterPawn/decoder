/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2020, Peter Haemmerlein (peterpawn@yourfritz.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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
	*cipher_keyLen = AES256_KEY_SIZE;
	*cipher_ivLen = AES_BLOCK_SIZE;
	*cipher_blockSize = AES_BLOCK_SIZE;
}

EXPORTED	void	CryptoCleanup(void)
{
	return;
}

EXPORTED	CipherContext *	CipherContextNew(void)
{
	CipherContext *		ctx = malloc(sizeof(CipherContext));

	if (!ctx)
		return NULL;

	memset(ctx, 0, sizeof(CipherContext));
	ctx->cipher_mode = CipherTypeValue;

	return ctx;
}

// initialize a cipher context

EXPORTED	CipherContext *	CipherInit(CipherContext * ctx, CipherMode mode, char * key, char * iv, UNUSED bool padding)
{
	CipherContext	*cipherCTX = NULL;

	if (ctx)
	{
		if (key && iv) /* reset context first */
		{
			memset(ctx, 0, sizeof(CipherContext));
			cipherCTX = ctx;
		}
	}
	else
	{
		cipherCTX = CipherContextNew();

		if (!cipherCTX)
			returnError(OSSL_CIPHER_ERR, NULL);
	}

	if (!key && !iv)
		return cipherCTX;

	aes256_set_decrypt_key(&(cipherCTX->cbc_context.ctx), (uint8_t *) key);
	cipherCTX->cipher_mode = mode;
	if (mode == CipherTypeValue)
		CBC_SET_IV(&(cipherCTX->cbc_context), iv);

	return cipherCTX;
}

// cleanup a context and free the used memory

EXPORTED 	CipherContext *	CipherCleanup(CipherContext * ctx)
{	
	if (!ctx)
		return NULL;

	memset(ctx, 0, sizeof(CipherContext));
	free(ctx);

	return NULL;
}

// update a CBC context

EXPORTED	bool	CipherUpdate(CipherContext * ctx, char *output, size_t *outputSize, char *input, size_t inputSize)
{
	if (!ctx)
		return false;

//	if (*outputSize < inputSize)
//	{
//		setError(BUF_TOO_SMALL);
//		return false;
//	}

	size_t				inSize = (inputSize - (inputSize % *cipher_blockSize));

	if (ctx->cipher_mode == CipherTypeValue)
	{
		CBC_DECRYPT(&(ctx->cbc_context), aes256_decrypt, inSize, (uint8_t *) output, (uint8_t *) input);
		*outputSize = inSize;
	}
	else
	{
		aes256_decrypt(&(ctx->cbc_context.ctx), inSize, (uint8_t *) output, (uint8_t *) input);
		*outputSize = inSize;
	}

	return true;
}

// finalize a decryption

EXPORTED	bool	CipherFinal(CipherContext * ctx, UNUSED char *output, UNUSED size_t *outputSize)
{
	if (!ctx)
		return false;

	*outputSize = 0;

	return true;
}

// digest functions

// get digest value length

EXPORTED	void	DigestSizes()
{
	*digest_blockSize = MD5_DIGEST_SIZE;
}

// initialize a digest context

EXPORTED	DigestContext * DigestInit()
{
	DigestContext *		ctx;

	ctx = malloc(sizeof(DigestContext));
	if (!ctx)
		setError(OSSL_DIGEST_ERR);
	else
		md5_init(ctx);
	return ctx;
}

// update a digest context from the specified buffer

EXPORTED	bool	DigestUpdate(DigestContext * ctx, char * buffer, size_t bufferSize)
{
	if (!ctx)
		return false;

	md5_update(ctx, bufferSize, (const uint8_t *) buffer);

	return true;
}

// finalize a digest value

EXPORTED	bool	DigestFinal(DigestContext * ctx, char * output)
{
	if (!ctx)
		return false;

	md5_digest(ctx, *digest_blockSize, (uint8_t *) output);
	return true;
}

// cleanup a digest context, free resources

EXPORTED	DigestContext *	DigestCleanup(DigestContext * ctx)
{
	if (!ctx)
		return NULL;
	memset(ctx, 0, sizeof(DigestContext));
	free(ctx);
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
