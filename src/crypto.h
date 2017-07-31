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

#ifndef CRYPTO_H

#define CRYPTO_H

#include "common.h"

// EVP types 

#define CipherContext			EVP_CIPHER_CTX
#define DigestContext			EVP_MD_CTX
#define CipherTypeValue			crypto_EVP_aes_256_cbc()
#define CipherTypeFile 			crypto_EVP_aes_256_ecb()
#define DigestType				crypto_EVP_md5()
#define MAX_DIGEST_SIZE			EVP_MAX_MD_SIZE

#ifndef CRYPTO_C

// various sizes

extern size_t *	cipher_keyLen;
extern size_t *	cipher_ivLen;
extern size_t *	cipher_blockSize;
extern size_t *	digest_blockSize;

#endif

// cipher and digest functions

void			CipherSizes();
CipherContext *	CipherInit(CipherContext * ctx, EVP_CIPHER * type, char * key, char * iv, bool padding);
CipherContext *	CipherCleanup(CipherContext * ctx);
bool			CipherUpdate(CipherContext * ctx, char *output, size_t *outputSize, char *input, size_t inputSize);
bool			CipherFinal(CipherContext * ctx, char *output, size_t *outputSize);

void			DigestSizes(void);
DigestContext *	DigestInit();
DigestContext * DigestCleanup(DigestContext * ctx);
bool			DigestUpdate(DigestContext * ctx, char * buffer, size_t bufferSize);
bool			DigestFinal(DigestContext * ctx, char * output);
size_t			Digest(char *buffer, size_t bufferSize, char *digest, size_t digestSize);

EVP_CIPHER *	crypto_EVP_aes_256_cbc(void);
EVP_CIPHER *	crypto_EVP_aes_256_ecb(void);
EVP_MD *		crypto_EVP_md5(void);
void			crypto_EVP_cleanup(void);
EVP_CIPHER_CTX *	crypto_EVP_CIPHER_CTX_new(void);

#endif
