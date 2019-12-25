/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2019, Peter Haemmerlein (peterpawn@yourfritz.de)
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

#ifndef CRYPTO_H

#define CRYPTO_H

#include "common.h"

// EVP types 

#define CipherContext			EVP_CIPHER_CTX
#define DigestContext			EVP_MD_CTX
#define MAX_DIGEST_SIZE			EVP_MAX_MD_SIZE

typedef enum {
	CipherTypeValue,	/* use CBC mode */
	CipherTypeFile,		/* use ECB mode */
} CipherMode;

#ifndef CRYPTO_C

// various sizes

extern size_t *	cipher_keyLen;
extern size_t *	cipher_ivLen;
extern size_t *	cipher_blockSize;
extern size_t *	digest_blockSize;

#endif

// cipher and digest functions

void			CipherSizes();
CipherContext *	CipherInit(CipherContext * ctx, CipherMode mode, char * key, char * iv, bool padding);
CipherContext *	CipherCleanup(CipherContext * ctx);
bool			CipherUpdate(CipherContext * ctx, char *output, size_t *outputSize, char *input, size_t inputSize);
bool			CipherFinal(CipherContext * ctx, char *output, size_t *outputSize);

void			DigestSizes(void);
DigestContext *	DigestInit();
DigestContext * DigestCleanup(DigestContext * ctx);
bool			DigestUpdate(DigestContext * ctx, char * buffer, size_t bufferSize);
bool			DigestFinal(DigestContext * ctx, char * output);
size_t			Digest(char *buffer, size_t bufferSize, char *digest, size_t digestSize);

void			CryptoCleanup(void);
CipherContext *	CipherContextNew(void);

#endif
