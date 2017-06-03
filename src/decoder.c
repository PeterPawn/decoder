/*
 * vim: set tabstop=4 syntax=c :
 *
 * multi-call utility to provide the same functionality as the shell scripts
 * from the 'decode_passwords' project, but as a lightning-fast binary
 * implementation
 *
 * Copyright (C) 2014-2017, Peter Haemmerlein (peterpawn@yourfritz.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * PLEASE KEEP IN MIND, THAT THE SAME LICENSE EXCEPTIONS ARE USED FOR THIS 
 * CODE, THAT WERE ESTABLISHED FOR THE SHELL SCRIPTS IN THIS PROJECT. HAVE 
 * A LOOK INTO THE README FILE AT THE PROJECT ROOT DIRECTORY.
 *
 * Otherwise this program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, please look for the file LICENSE.
 */

#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <getopt.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <openssl/evp.h>
#include "decoder.h"

#define EXPORTED                        __attribute__((__visibility__("default")))
#define UNUSED                          __attribute__((unused))

// global verbosity setting

static decoder_verbosity_t 				__decoder_verbosity = VERBOSITY_NORMAL;

#define verbosity_options_long			{ "verbose", no_argument, NULL, 'v' },\
										{ "quiet", no_argument, NULL, 'q' },\
										{ "help", no_argument, NULL, 'h' }
#define verbosity_options_short			"qvh"
#define check_verbosity_options_short()	case 'v':\
											__decoder_verbosity = VERBOSITY_VERBOSE;\
											break;\
										case 'q':\
											__decoder_verbosity = VERBOSITY_SILENT;\
											break
#define invalid_option(opt)				default:\
											errorMessage("Invalid option '%c' specified.\a\n", (char) opt);\
											exit(EXIT_FAILURE)
#define help_option(function)			case 'h':\
											usageScreen_##function ();\
											exit(EXIT_FAILURE)
#define getopt_message_displayed(cmd)	case '?':\
											usageScreen_##cmd ();\
											exit(EXIT_FAILURE);

// global error state

static decoder_error_t					__decoder_error = DECODER_ERROR_NOERROR;

#define setError(err)                   __decoder_error = DECODER_ERROR_##err
#define resetError()					setError(NOERROR)
#define returnError(err,value)          { setError(err); return (value); }
#define getError()						(__decoder_error)
#define getErrorText(err)				(__decoder_error_text[err])
#define isAnyError()					(getError() != DECODER_ERROR_NOERROR)
#define isError(err)					(getError() == DECODER_ERROR_##err)
#define isVerbose()						(__decoder_verbosity == VERBOSITY_VERBOSE)
#define errorMessage(...)				if (__decoder_verbosity != VERBOSITY_SILENT) fprintf(stderr, ##__VA_ARGS__)
#define verboseMessage(...)				if (__decoder_verbosity == VERBOSITY_VERBOSE) fprintf(stderr, ##__VA_ARGS__)

// callable functions table

static commandEntry_t	commands[] = {
	{ .name = "b32dec", .ep = &b32dec_main },
	{ .name = "b32enc", .ep = &b32enc_main },
	{ .name = "b64dec", .ep = &b64dec_main },
	{ .name = "b64enc", .ep = &b64enc_main },
	{ .name = "hexdec", .ep = &hexdec_main },
	{ .name = "hexenc", .ep = &hexenc_main },
	{ .name = "user_password", .ep = &user_password_main },
	{ .name = "device_password", .ep = &device_password_main },
	{ .name = "password_from_device", .ep = &password_from_device_main },
	{ .name = "decode_secret", .ep = &decode_secret_main },
	{ .name = "decode_secrets", .ep = &decode_secrets_main },
	{ .name = "decode_passwords", .ep = &decode_secrets_main },
	{ .name = "decode_export", .ep = &decode_export_main },
	{ .name = NULL, .ep = NULL }
};

// display usage help

void	usageScreen(void)
{
	errorMessage("decoder for AVM's cipher implementation\n");
}

void	usageScreen_b32dec(void)
{
	errorMessage("help for b32dec\n");
}

void	usageScreen_b32enc(void)
{
	errorMessage("help for b32enc\n");
}

void	usageScreen_b64dec(void)
{
	errorMessage("help for b64dec\n");
}

void	usageScreen_b64enc(void)
{
	errorMessage("help for b64enc\n");
}

void	usageScreen_hexdec(void)
{
	errorMessage("help for hexdec\n");
}

void	usageScreen_hexenc(void)
{
	errorMessage("help for hexenc\n");
}

void	usageScreen_user_password(void)
{
	errorMessage("help for user_password\n");
}

void	usageScreen_device_password(void)
{
	errorMessage("help for device_password\n");
}

void	usageScreen_decode_secret(void)
{
	errorMessage("help for decode_secret\n");
}

void	usageScreen_decode_secrets(void)
{
	errorMessage("help for decode_secrets\n");
}

void	usageScreen_decode_exports(void)
{
	errorMessage("help for decode_exports\n");
}

// Base32 encoding table

static 	char * UNUSED	base32Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456";

// Base64 encoding table

static 	char * UNUSED	base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// hexadecimal translation table

static 	char * UNUSED	hexTable = "0123456789ABCDEF";

// static size values
static	size_t			cipher_keyLen, cipher_ivLen, cipher_blockSize;

// urlader environment file name

static	char * UNUSED	environmentFileName = URLADER_ENV_PATH;

// subfunctions

memoryBuffer_t *	memoryBufferNew(size_t size)
{
	memoryBuffer_t	*new = (memoryBuffer_t *) malloc(size);
	if (new)
	{
		memset(new, 0, size);
		new->size = size;
	}
	return new;
}

memoryBuffer_t *	memoryBufferFreeChain(memoryBuffer_t *start)
{
	memoryBuffer_t	*curr = start;
	
	while (curr)
	{
		memoryBuffer_t	*next = curr->next;
		
		memset(curr, 0, curr->size);
		free(curr);
		curr = next;
	}
	return NULL;
}

memoryBuffer_t *	memoryBufferReadFile(FILE * file, size_t chunkSize)
{
	size_t			allocSize;
	memoryBuffer_t	*top = NULL;
	memoryBuffer_t	*curr = NULL;
	char *			data;
	size_t			read;
	size_t			toRead;

	allocSize = (chunkSize > 0 ? chunkSize : 8 * 1024);
	allocSize += sizeof(memoryBuffer_t);
	top = memoryBufferNew(allocSize);
	curr = top;
	data = top->data;
	toRead = curr->size - curr->used - sizeof(memoryBuffer_t);
	while ((read = fread(data, 1, toRead, file)) > 0)
	{
		curr->used += read;
		data += read;
		if ((read == toRead) && (curr->used == curr->size - sizeof(memoryBuffer_t))) /* buffer is full */
		{
			memoryBuffer_t	*next = memoryBufferNew(allocSize);
			if (!next)
			{
				setError(STDIN_BUFFER_ERR);
				top = memoryBufferFreeChain(top);
				break;
			}
			curr->next = next;
			next->prev = curr;
			curr = next;
			data = curr->data;
			toRead = curr->size - sizeof(memoryBuffer_t);
		}
	}
	if (!top->used)
	{	
		setError(NOERROR);
		free(top);
		top = NULL;
	}
	return top;
}

char *	memoryBufferFindString(memoryBuffer_t * *buffer, size_t *offset, char *find, size_t findSize, bool *split)
{
	memoryBuffer_t	*current = *buffer;
	
	while (current)
	{
		char *		chk = current->data + *offset;
		size_t		remaining = current->used - *offset;

		while (remaining > 0)
		{
			if (remaining < findSize)
			{
				if (strncmp(chk, find, remaining) == 0)
				{
					if (current->next)
					{
						if (strncmp(current->data, find + remaining, findSize - remaining) == 0) /* match across buffers */
						{
							*buffer = current;
							*offset = (chk - current->data);
							*split = true;
						}
					}
					else /* partial match at end of data */
						return NULL;
				}
				chk++;
				remaining--;
				if (remaining == 0) /* next buffer, if any */
				{
					if (current->next)
					{
						current = current->next;
						remaining = current->used;
						chk = current->data;
					}
					else /* no match */
						return NULL;
				}				
			}
			else
			{
				if (strncmp(chk, find, findSize) == 0)
				{
					*buffer = current;
					*offset = (chk - current->data);
					*split = false;
					return chk;
				}	
				chk++;
				remaining--;
			}
		}
	}
	return NULL;	
}

char *	memoryBufferAdvancePointer(memoryBuffer_t * *buffer, size_t *lastOffset, size_t offset)
{
	memoryBuffer_t	*current = *buffer;
	uint32_t		advance = offset;
	
	while (current)
	{
		if ((current->used - *lastOffset) < advance) /* next buffer */
		{
			if (current->next)
			{
				advance -= (current->used - *lastOffset);
				current = current->next;
				*buffer = current;
				*lastOffset = 0;
				continue;
			}
		}
		*lastOffset += advance;
		return (current->data + *lastOffset);
	}
	return NULL;	
}

char *	memoryBufferSearchValueEnd(memoryBuffer_t * *buffer, size_t *offset, size_t * size, bool *split)
{
	memoryBuffer_t	*current = *buffer;
	size_t  		curOffset = *offset;
	size_t			count = 0;
	char *			cur = current->data + curOffset;

	*split = false;	
	while (current)
	{
		if (current->used <= curOffset) /* next buffer */
		{
			if (current->next)
			{
				current = current->next;
				*buffer = current;
				*offset = 0;
				cur = current->data;
				*split = true;
			}
		}
		if ((*cur >= 'A' && *cur <= 'Z') || (*cur >= '1' && *cur <= '6'))
		{
			count++;
			cur++;
			curOffset++;
		}
		else /* character outside of our Base32 set found */
		{
			*buffer = current;
			*offset = curOffset;
			*size = count;
			return cur;
		}
	}
	return NULL;	
}

// scan memory buffer and replace occurences of encrypted data while writing data to STDOUT

bool	memoryBufferProcessFile(memoryBuffer_t * *buffer, size_t offset, char * key, FILE * out)
{
	CipherContext 		*ctx = CipherInit(NULL, NULL, NULL);
	memoryBuffer_t 		*currentBuffer = *buffer;
	size_t				currentOffset = offset;
	
	while (currentBuffer)
	{
		memoryBuffer_t	*found = currentBuffer;
		size_t			foundOffset = currentOffset;
		bool			split = false;
		char *			cipherTextStart;
	
		if ((cipherTextStart = memoryBufferFindString(&found, &foundOffset, "$$$$", 4, &split)) != NULL) /* encrypted data exists */
		{
			while (currentBuffer && (currentBuffer != found)) /* output data crosses at least one buffer boundary */
			{
				if (fwrite(currentBuffer->data + currentOffset, currentBuffer->used - currentOffset, 1, out) != 1)
				{
					setError(WRITE_FAILED);
					currentBuffer = NULL;
					break;
				}
				currentBuffer = currentBuffer->next;
				currentOffset = 0;
			}
			if (currentBuffer)
			{
				if (fwrite(currentBuffer->data + currentOffset, foundOffset - currentOffset, 1, out) != 1)
				{
					setError(WRITE_FAILED);
					currentBuffer = NULL;
					break;
				}
				currentOffset = foundOffset;
			}
			cipherTextStart = memoryBufferAdvancePointer(&currentBuffer, &currentOffset, 4);
			found = currentBuffer;
			foundOffset = currentOffset;

			size_t		valueSize;
			char *		cipherTextEnd = memoryBufferSearchValueEnd(&found, &foundOffset, &valueSize, &split);
			if (cipherTextEnd)
			{
				char *	copy;
				char *	cipherText = (char *) malloc(valueSize + 1);
			
				memset(cipherText, 0, valueSize + 1);
				copy = cipherText;
				while (currentBuffer && (currentBuffer != found))
				{
					memcpy(copy, currentBuffer->data + currentOffset, currentBuffer->used - currentOffset);
					copy += (currentBuffer->used - currentOffset);
					currentBuffer = currentBuffer->next;
					currentOffset = 0;
				}
				memcpy(copy, currentBuffer->data + currentOffset, foundOffset - currentOffset);
				*(copy + valueSize) = 0;

				if (!DecryptValue(ctx, cipherText, valueSize, out, NULL, key, true)) /* unable to decrypt, write data as is */
				{
					verboseMessage(" -> decryption failed\n");
					if (fwrite("$$$$", 4, 1, out) == 1)
					{
						if (fwrite(currentBuffer->data + currentOffset, currentBuffer->used - currentOffset, 1, out) != 1)
						{
							setError(WRITE_FAILED);
							currentBuffer = NULL;
						}
					}
					else
					{
						setError(WRITE_FAILED);
						currentBuffer = NULL;
					}
				}
				else
				{
					currentBuffer = found;
					currentOffset = foundOffset;
				}
				free(cipherText);
			}
		}
		else /* no more encrypted data, write remaining buffers */
		{
			while (currentBuffer)
			{
				if (fwrite(currentBuffer->data + currentOffset, currentBuffer->used - currentOffset, 1, out) != 1)
				{
					setError(WRITE_FAILED);
					currentBuffer = NULL;
					break;
				}
				currentBuffer = currentBuffer->next;
				currentOffset = 0;
			}
		}
	}

	ctx = CipherCleanup(ctx);

	return !isAnyError();

}

// conversion functions

size_t	base32ToBinary(char *base32, size_t base32Size, char *binary, size_t binarySize)
{
	uint32_t        offset = 0;
	uint32_t		outOffset = 0;
	size_t			b32Size = (base32Size == (size_t) -1 ? strlen(base32) : base32Size);
	
	if (b32Size % 8)
		returnError(INV_B32_SIZE, 0);
	if ((b32Size * 5 / 8) > binarySize)
		returnError(BUF_TOO_SMALL, (b32Size * 5 / 8));
	while (offset < b32Size && outOffset < binarySize)
	{
		uint32_t		bits = 0;
		uint32_t		value = 0;

		for (uint32_t i = 0; i < 8; i++)
		{
			char 	c = *(base32 + offset + i);
	
			if (c >= 'A' && c <= 'Z')
				c = c - 'A';
			else if (c >= '1' && c <= '6')
				c = c - '1' + 26;
			else
				returnError(INV_B32_DATA, 0);

			value = (value << 5) + c;
			bits += 5;

			if (bits >= 8)
			{
				*(binary + outOffset) = (char) (value >> (bits - 8));
				bits -= 8;
				value = value % (1 << bits);
				outOffset++;
			}
		}
		offset += 8;
	}
	return outOffset;
}

size_t	binaryToBase32(char *binary, size_t binarySize, char *base32, size_t base32Size)
{
	uint32_t		offset = 0;
	uint32_t		outOffset = 0;

	if (binarySize % 5)
		returnError(INV_B32_ENC_SIZE, 0);
	if ((binarySize * 8 / 5) > base32Size)
		returnError(BUF_TOO_SMALL, 0);
	while (offset < binarySize && outOffset < base32Size)
	{
		uint32_t	bits = 0;
		uint32_t	value = 0;

		for (uint32_t i = 0; i < 5; i++)
		{
			value = (value << 8) + (*(binary + offset + i) & 0xFF);
			bits += 8;
			while (bits >= 5)
			{
				*(base32 + outOffset) = base32Table[(value >> (bits - 5))];
				bits -= 5;
				value = value % (1 << bits);
				outOffset++;
			}
		}
		offset += 5;
	}
	return outOffset;
}

size_t	base64ToBinary(char *base64, size_t base64Size, char *binary, size_t binarySize, bool pad)
{
	uint32_t        offset = 0;
	uint32_t		outOffset = 0;
	size_t			b64Size = (base64Size == (size_t) -1 ? strlen(base64) : base64Size);
	bool			filler = false;
	size_t			bSize = (b64Size * 3 / 4);
	uint32_t		bits = 0;
	uint32_t		value = 0;
	
	if (b64Size % 4)
	{
		if (pad)	
			bSize = ((b64Size / 4) + 1) * 3;
		else
			returnError(INV_B64_SIZE, 0);
	}
	if (bSize > binarySize)
		returnError(BUF_TOO_SMALL, bSize);
	while (offset < b64Size && outOffset < binarySize)
	{
		value = 0;
		for (uint32_t i = 0; i < 4 && offset < b64Size; i++)
		{
			char 	c = *(base64 + offset++);
	
			if (c >= 'A' && c <= 'Z')
				c = c - 'A';
			else if (c >= 'a' && c <= 'z')
				c = c - 'a' + 26;
			else if (c >= '0' && c <= '9')
				c = c - '0' + 52;
			else if (c == '+')
				c = 62;
			else if (c == '/')
				c = 63;
			else if (c == '=')
			{
				filler = true;
				c = 0;
			}
			else
				returnError(INV_B64_DATA, 0);

			value = (value << 6) + c;
			bits += 6;

			if (bits == 24)
			{
				*(binary + outOffset) = (char) (value >> 16);
				*(binary + outOffset + 1) = (char) ((value >> 8) & 0xFF);
				*(binary + outOffset + 2) = (char) (value & 0xFF);
				bits = 0;
				outOffset += 3;
			}
		}
		if (filler && offset < b64Size)
			returnError(INV_B64_DATA, 0);
	}
	if (bits > 0)
	{
		if (pad)
		{
			if (bits == 12)
				value = value << 12;
			else if (bits == 18)
				value = value << 6;
			else /* at least two characters are needed for one byte */
				returnError(INV_B64_SIZE, 0);
		}
		else
			returnError(INV_B64_SIZE, 0);
		*(binary + outOffset) = (char) (value >> 16);
		outOffset++;
		*(binary + outOffset) = (char) ((value >> 8) & 0xFF);
		outOffset++;
		if (bits > 12)
		{
			if (pad)
			{
				*(binary + outOffset) = 0;
				outOffset++;
			}
		}
	}
	return outOffset;
}

size_t	binaryToBase64(char *binary, size_t binarySize, char *base64, size_t base64Size, bool pad)
{
	uint32_t		offset = 0;
	uint32_t		outOffset = 0;
	uint32_t		bSize = (binarySize * 4 / 3);
	uint32_t		bits = 0;
	uint32_t		value = 0;

	if (binarySize % 3)
		bSize = ((binarySize / 3 ) + 1) * 4;
	if (bSize > base64Size)
		returnError(BUF_TOO_SMALL, 0);
	while (offset < binarySize && outOffset < base64Size)
	{
		value = 0;
		for (uint32_t i = 0; i < 3 && offset < binarySize; i++)
		{
			value = (value << 8) + (*(binary + offset++) & 0xFF);
			bits += 8;
			if (bits == 24)
			{
				*(base64 + outOffset) = base64Table[(value >> 18)];
				*(base64 + outOffset + 1) = base64Table[((value >> 12) % 64)];
				*(base64 + outOffset + 2) = base64Table[((value >> 6) % 64)];
				*(base64 + outOffset + 3) = base64Table[(value % 64)];
				bits = 0;
				outOffset += 4;
			}
		}
	}
	if (bits > 0) /* finalize data */
	{
		value = value << (24 - bits);
		*(base64 + outOffset) = base64Table[(value >> 18)];
		outOffset++;
		*(base64 + outOffset) = base64Table[((value >> 12) % 64)];
		outOffset++;
		if (bits == 16)
		{
			*(base64 + outOffset) = base64Table[((value >> 6) % 64)];
			outOffset++;
			if (pad)
			{
				*(base64 + outOffset) = '=';
				outOffset++;
			}
		}
		else
		{
			if (pad)
			{
				*(base64 + outOffset) = '=';
				outOffset++;
				*(base64 + outOffset) = '=';
				outOffset++;
			}
		}
	}
	return outOffset;
}

size_t 	hexadecimalToBinary(char *input, size_t inputSize, char *output, size_t outputSize)
{
	uint32_t        offset = 0;
	uint32_t		outOffset = 0;
	size_t			inSize = (inputSize == (size_t) -1 ? strlen(input) : inputSize);
	uint32_t		value = 0;
	bool			high = true;
	
	if (inSize & 1)
		returnError(INV_HEX_SIZE, 0);
	if ((inSize / 2) > outputSize)
		returnError(BUF_TOO_SMALL, (inSize / 2));
	while (offset < inSize && outOffset < outputSize)
	{
		char		c = *(input + offset++) & 0xFF;
	
		if (c >= 'A' && c <= 'F')
		{
			c = c - 'A' + 10;
		}
		else if (c >= 'a' && c <= 'f')
		{
			c = c - 'a' + 10;
		}
		else if (c >= '0' && c <= '9')
		{
			c -= '0';
		}
		else
		{
			returnError(INV_HEX_DATA, 0);
		}
		if (high)
		{
			value = c << 4;
			high = false;
		}
		else
		{
			value = value + c;
			high = true;
			*(output + outOffset++) = value;
		}
	}
	return outOffset;
}

size_t	binaryToHexadecimal(char *input, size_t inputSize, char *output, size_t outputSize)
{
	uint32_t			index = 0;

	if ((inputSize * 2) > (outputSize - 1))
		returnError(BUF_TOO_SMALL, (inputSize * 2));
	while ((index < inputSize) && ((index * 2) < (outputSize - 1)))
	{
		*(output + (index * 2)) = hexTable[(((*(input + index)) >> 4) & 0x0F)];
		*(output + (index * 2) + 1) = hexTable[((*(input + index)) & 0x0F)];
		index++;
	}
	return (index * 2);
}

// output formatting

char *	wrapOutput(bool wrapLines, uint32_t lineSize, uint32_t *charsOnLine, uint32_t *toWrite, char *output)
{
	uint32_t			remOnLine = lineSize - *charsOnLine;
	char *				out = output;

	if (wrapLines && (*toWrite > remOnLine)) /* wrap on lineSize */
	{
		if ((remOnLine > 0) && (fwrite(out, remOnLine, 1, stdout) != 1)) /* remaining line */
			returnError(WRITE_FAILED, out);
		out += remOnLine;
		*toWrite -= remOnLine;
		*charsOnLine = 0;
		if (fwrite("\n", 1, 1, stdout) != 1) /* append newline */
			returnError(WRITE_FAILED, 0);
		while (*toWrite > lineSize)
		{
			if (fwrite(out, lineSize, 1, stdout) != 1)
				returnError(WRITE_FAILED, 0);
			*toWrite -= lineSize;
			out += lineSize;
			if (fwrite("\n", 1, 1, stdout) != 1) /* append newline */
				returnError(WRITE_FAILED, 0);
		}
	}
	return out;
}

// free memory after clearing its content

void *	clearMemory(void * buffer, size_t size, bool freeBuffer)
{
	if (buffer)
	{
		memset(buffer, 0, size);
		if (freeBuffer) free(buffer);
	}
	return NULL;
}

// cipher functions

CipherContext *	CipherInit(CipherContext * ctx, char * key, char * iv)
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

void	CipherSizes()
{
	cipher_keyLen = cipher_ivLen = cipher_blockSize = (size_t) -1;
	cipher_keyLen = EVP_CIPHER_key_length(CipherType);
	cipher_ivLen = EVP_CIPHER_iv_length(CipherType);
	cipher_blockSize = EVP_CIPHER_block_size(CipherType);
}

CipherContext *	CipherCleanup(CipherContext * ctx)
{	
	if (!ctx)
		return NULL;
	EVP_CIPHER_CTX_cleanup(ctx);
	EVP_CIPHER_CTX_free(ctx);
	return NULL;
}

bool	CipherUpdate(CipherContext * ctx, char *output, size_t *outputSize, char *input, size_t inputSize)
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

bool	DecryptValue(CipherContext * ctx, char * cipherText, size_t valueSize, FILE * out, char * outBuffer, char * key, bool escaped)
{
	size_t			cipherBufSize = base32ToBinary(cipherText, valueSize, NULL, 0);
	size_t			cipherSize;
	char *			cipherBuffer = (char *) malloc(cipherBufSize + cipher_blockSize);
	size_t			decryptedSize = 0;
	char *			decryptedBuffer = (char *) malloc(cipherBufSize + cipher_blockSize);
	CipherContext 	*localCtx;

	cipherSize = base32ToBinary(cipherText, (size_t) -1, (char *) cipherBuffer, cipherBufSize + cipher_blockSize);
	
	localCtx = (ctx ? ctx : EVP_CIPHER_CTX_new());
	CipherInit(localCtx, key, cipherBuffer);
	verboseMessage("found cipher text '%s' -> ", cipherText);
	if (!(cipherSize % cipher_blockSize))
		cipherSize++;
	if (CipherUpdate(localCtx, decryptedBuffer, &decryptedSize, cipherBuffer + cipher_ivLen, cipherSize - cipher_ivLen))
	{
		char *		value;
		size_t		valueSize = 0;
		bool		isString = false;

		if (DigestCheckValue(decryptedBuffer, decryptedSize, &value, &valueSize, &isString))
		{
			if (out && valueSize && escaped)
			{
				int	start = 0;

				verboseMessage("decrypted to '%s'\n", value);
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
					verboseMessage("decrypted to 0x%s\n", hexBuffer);
					free(hexBuffer);
				}
				else
					verboseMessage("error displaying value, but decryption was successful\n");
			}
		}
		else
		{
			setError(DECRYPT_ERR);
			verboseMessage("decrypt failed\n");
		}
	}
	cipherBuffer = clearMemory(cipherBuffer, cipherBufSize, true);
	decryptedBuffer = clearMemory(decryptedBuffer, cipherBufSize + cipher_blockSize, true);
	if (!ctx)
		localCtx = CipherCleanup(localCtx);

	return !isAnyError();
}

// digest functions

DigestContext * DigestInit()
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

bool	DigestUpdate(DigestContext * ctx, char * buffer, size_t bufferSize)
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

bool	DigestFinal(DigestContext * ctx, char * output)
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

DigestContext *	DigestCleanup(DigestContext * ctx)
{
	if (!ctx)
		return NULL;
	EVP_MD_CTX_destroy(ctx);
	return NULL;
}

size_t	Digest(char *buffer, size_t bufferSize, char *digest, size_t digestSize)
{
	size_t		size = 0;

	resetError();

	DigestContext	*ctx = DigestInit();

	if (isAnyError())
		return false;

	size = DigestLength(ctx);
	if (digestSize < size)
		setError(BUF_TOO_SMALL);
	else
	{
		if (DigestUpdate(ctx, buffer, bufferSize))
		{
			DigestFinal(ctx, digest);
		}		
	}
	ctx = DigestCleanup(ctx);
	return (isAnyError() ? 0 : size);
}

size_t	DigestLength(DigestContext * ctx)
{
	if (!ctx)
		return (size_t) -1;
	return EVP_MD_CTX_size(ctx);
}

bool	DigestCheckValue(char *buffer, size_t bufferSize, char * *value, size_t * dataLen, bool * string)
{
	char				hash[MAX_DIGEST_SIZE];
	size_t				hashLen = sizeof(hash);

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

// password generation from device properties

bool	keyFromDevice(char * hash, size_t * hashSize, bool forExport)
{
	FILE *				environment = fopen(environmentFileName, "r");

	if (!environment)
	{
		errorMessage("Error opening environment file on procfs (%s).\n\nAre we really running on a FRITZ!OS device?\a\n", environmentFileName);
		return false;	
	}
	
	memoryBuffer_t *	env = memoryBufferReadFile(environment, 8 * 1024);

	fclose(environment);

	if (!env)
	{
		errorMessage("Error reading environment file on procfs (%s).\n\nAre we really running on a FRITZ!OS device?\a\n", environmentFileName);
		return false;
	}

	DigestContext	 	*ctx = DigestInit();

	struct variables {
		char *			name;
		char *			show;
		char *			append;
		bool			errorIfMissing;
		bool			export;
	}					envVariables[] = {
						{ .name = URLADER_SERIAL_NAME"\t", .show = URLADER_SERIAL_NAME, .append = "\n", .errorIfMissing = true, .export = true },
						{ .name = URLADER_MACA_NAME"\t", .show = URLADER_MACA_NAME, .append = "\n", .errorIfMissing = true, .export = true },
						{ .name = URLADER_WLANKEY_NAME"\t", .show = URLADER_WLANKEY_NAME, .append = NULL, .errorIfMissing = true, .export = false },
						{ .name = URLADER_TR069PP_NAME"\t", .show = URLADER_TR069PP_NAME, .append = NULL, .errorIfMissing = false, .export = false },
						{ .name = NULL, .show = NULL, .append = NULL, .errorIfMissing = false },
	};
	struct variables 	*var = envVariables;

	while (var && var->name)
	{
		memoryBuffer_t	*currentBuffer = env;
		size_t			currentOffset = 0;
		char *			name;
		bool			split;
	
		name = memoryBufferFindString(&currentBuffer, &currentOffset, var->name, strlen(var->name), &split);
		if (name)
		{	
			char *		value = memoryBufferAdvancePointer(&currentBuffer, &currentOffset, strlen(var->name));
			char *		curr = value;
			size_t		valueSize = 0;

			verboseMessage("found device property '%s' with value '", var->show);
			while (currentBuffer && *curr != '\n') 
			{
				if (currentBuffer->used <= currentOffset) /* next buffer */
				{
					if (currentBuffer->next)
					{
						DigestUpdate(ctx, value, valueSize);
						if (isVerbose())
						{
							while (*value != '\n' && valueSize > 0)
							{
								verboseMessage("%c", *value);
								value++;
								valueSize--;
							}
							verboseMessage("'\n");
						}
						currentBuffer = currentBuffer->next;
						value = currentBuffer->data;
						valueSize = 0;
						curr = value;
					}
					else
						break;
				}
				else
				{
					curr++;
					currentOffset++;
					valueSize++;
				}
			}
			DigestUpdate(ctx, value, valueSize);
			if (var->append)
				DigestUpdate(ctx, var->append, strlen(var->append));
			if (isVerbose())
			{
				while (*value != '\n' && valueSize > 0)
				{
					verboseMessage("%c", *value);
					value++;
					valueSize--;
				}
				verboseMessage("'\n");
			}
		}
		else
		{
			if (var->errorIfMissing == true)
			{
				errorMessage("Unable to read variable '%s' from environment file on procfs(%s).\n\nAre we really running on a FRITZ!OS device?\a\n", var->show, environmentFileName);
				setError(URLADER_ENV_ERR);
				break;
			}
			verboseMessage("device property '%s' does not exist.\n", var->show);
		}
		var++;
		if (forExport && !var->export)
			break;
	}

	env = memoryBufferFreeChain(env);

	*hashSize = DigestLength(ctx);
	DigestFinal(ctx, hash);
	ctx = DigestCleanup(ctx);
	
	return !isAnyError();
}

// password generation from properties

bool	keyFromProperties(char * hash, size_t * hashSize, char * serial, char * maca, char * wlanKey, char * tr069Passphrase)
{
	DigestContext	 	*ctx = DigestInit();

	DigestUpdate(ctx, serial, strlen(serial));
	DigestUpdate(ctx, "\n", 1);
	DigestUpdate(ctx, maca, strlen(maca));
	DigestUpdate(ctx, "\n", 1);
	if (wlanKey && *wlanKey)
		DigestUpdate(ctx, wlanKey, strlen(wlanKey));
	if (tr069Passphrase && *tr069Passphrase)
		DigestUpdate(ctx, tr069Passphrase, strlen(tr069Passphrase));
	*hashSize = DigestLength(ctx);
	DigestFinal(ctx, hash);
	ctx = DigestCleanup(ctx);

	return !isAnyError();
}

// callable functions of the multi-call binary

// 'b32dec' function - decode Base32 encoded data from STDIN to STDOUT

void b32dec_output(char * base32, bool hexOutput)
{
	char				binary[5];
	size_t				binarySize = base32ToBinary(base32, (size_t) -1, binary, sizeof(binary));
	char				hex[10];
	char *				out;
	size_t				outSize;

	if (isAnyError()) /* usually invalid characters */
	{
		if (isError(INV_B32_DATA))
		{
			errorMessage("Invalid data value encountered on STDIN.\a\n");
		}
		else if (isError(INV_B32_SIZE))
		{
			errorMessage("Invalid data size encountered on STDIN.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}
	if (hexOutput)
	{
		outSize = binaryToHexadecimal(binary, binarySize, hex, sizeof(hex));
		out = hex;
	}
	else
	{
		outSize = binarySize;
		out = binary;
	}
	if (fwrite(out, outSize, 1, stdout) != 1)
	{
		setError(WRITE_FAILED);
		errorMessage("Write to STDOUT failed.\a\n");
		exit(EXIT_FAILURE);
	}
}

int b32dec_main(int argc, char** argv, int argo)
{
	char				buffer[81];
	char *				input;
	char				base32[9];
	int					convUsed = 0;
	bool				hexOutput = false;

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
					break;

				check_verbosity_options_short();
				help_option(b32dec);
				getopt_message_displayed(b32dec);
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((input = fgets(buffer, sizeof(buffer), stdin)) != NULL)
	{
		input--;
		while (*(++input))
		{
			if (isspace(*input))
				continue;
			base32[convUsed++] = *input;
			if (convUsed == 8)
			{
				base32[convUsed] = 0;
				b32dec_output(base32, hexOutput);
				convUsed = 0;
			}
		}
	}	

	if (convUsed > 0) /* remaining data exist */
	{
		base32[convUsed] = 0;
		b32dec_output(base32, hexOutput);
	}
	
	return EXIT_SUCCESS;
}

// 'b32enc' function - encode binary data from STDIN to Base32 encoded on STDOUT

int b32enc_main(int argc, char** argv, int argo)
{
	bool				hexInput = false;
	bool				padInput = false;
	char				buffer[20];
	size_t				read = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-input", no_argument, NULL, 'x' },
			{ "pad-input", no_argument, NULL, 'p' },
		};
		char *			options_short = "xp" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexInput = true;
					break;

				case 'p':
					padInput = true;
					break;

				check_verbosity_options_short();
				help_option(b32enc);
				getopt_message_displayed(b32enc);
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
	{
		if (hexInput)
		{
			char	withoutSpaces[sizeof(buffer)];
			size_t	used = 0;
			char *	in;
			char *	out;
			int		i;
			size_t	more = read;

			in = buffer;
			out = withoutSpaces;
			while (more > 0)
			{
				for (i = more; i > 0; i--, in++)
				{
					if (isspace(*(in)))
						continue;
					*(out++) = *in;
					used++;
				}
				if (used == sizeof(withoutSpaces))
					break;
				more = fread(buffer, 1, sizeof(withoutSpaces) - used, stdin);
				if (more == 0)
					break;
				in = buffer;
			}
			read = hexadecimalToBinary(withoutSpaces, used, buffer, sizeof(buffer));
			if (read == 0) 
				break;
		}
		if ((read % 5))
		{
			if (padInput)
			{
				int		r = 5 - (read % 5);
				char *	pad = &buffer[read];

				for (int i = 0; i < r; i++)
				{
					*pad++ = 0;
					read++;
				}
			}
			else
			{
				setError(INV_B32_ENC_SIZE);
				read = 0;
				break;	
			}
		}
		
		char	base32[(sizeof(buffer) / 5 * 8) + 1]; /* one more byte for optional end of string */
		size_t	base32Size = binaryToBase32(buffer, read, base32, sizeof(base32) - 1);

		if (base32Size > 0)
		{
			if (fwrite(base32, base32Size, 1, stdout) != 1)
			{
				setError(WRITE_FAILED);
				errorMessage("Write to STDOUT failed.\a\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	
	if (isAnyError()) 
	{
		if (isError(INV_HEX_DATA))
		{
			errorMessage("Invalid hexadecimal data value encountered on STDIN.\a\n");
		}
		else if (isError(INV_HEX_SIZE))
		{
			errorMessage("Invalid hexadecimal data size encountered on STDIN.\a\n");
		}
		else if (isError(INV_B32_ENC_SIZE))
		{
			errorMessage("Invalid data size encountered on STDIN.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

// 'b64dec' function - decode Base64 encoded data from STDIN to STDOUT

void b64dec_output(char * base64, bool hexOutput, bool pad)
{
	char				binary[3];
	size_t				binarySize = base64ToBinary(base64, (size_t) -1, binary, sizeof(binary), pad);
	char				hex[6];
	char *				out;
	size_t				outSize;

	if (isAnyError()) /* usually invalid characters */
	{
		if (isError(INV_B64_DATA))
		{
			errorMessage("Invalid data value encountered on STDIN.\a\n");
		}
		else if (isError(INV_B64_SIZE))
		{
			errorMessage("Invalid data size encountered on STDIN.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}
	if (hexOutput)
	{
		outSize = binaryToHexadecimal(binary, binarySize, hex, sizeof(hex));
		out = hex;
	}
	else
	{
		outSize = binarySize;
		out = binary;
	}
	if (fwrite(out, outSize, 1, stdout) != 1)
	{
		setError(WRITE_FAILED);
		errorMessage("Write to STDOUT failed.\a\n");
		exit(EXIT_FAILURE);
	}
}

int b64dec_main(int argc, char** argv, int argo)
{
	char				buffer[80];
	char *				input;
	char				base64[5];
	int					convUsed = 0;
	bool				hexOutput = false;
	bool				padOutput = false;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-output", no_argument, NULL, 'x' },
			{ "pad-output", no_argument, NULL, 'p' },
		};
		char *			options_short = "xp" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexOutput = true;
					break;

				case 'p':
					padOutput = true;
					break;

				check_verbosity_options_short();
				help_option(b64dec);
				getopt_message_displayed(b64dec);
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((input = fgets(buffer, sizeof(buffer), stdin)) != NULL)
	{
		input--;
		while (*(++input))
		{
			if (isspace(*input))
				continue;
			base64[convUsed++] = *input;
			if (convUsed == 4)
			{
				base64[convUsed] = 0;
				b64dec_output(base64, hexOutput, padOutput);
				convUsed = 0;
			}
		}
	}	

	if (convUsed > 0) /* remaining data exist */
	{
		base64[convUsed] = 0;
		b64dec_output(base64, hexOutput, padOutput);
	}
	
	return EXIT_SUCCESS;
}

// 'b64enc' function - encode binary data from STDIN to Base64 encoded on STDOUT

int b64enc_main(int argc, char** argv, int argo)
{
	bool				hexInput = false;
	bool				padOutput = false;
	bool				wrapLines = false;
	uint32_t			lineSize = 76;
	uint32_t			charsOnLine = 0;
	char				buffer[120];
	size_t				read = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-input", no_argument, NULL, 'x' },
			{ "pad-output", no_argument, NULL, 'p' },
			{ "wrap-lines", optional_argument, NULL, 'w' },
		};
		char *			options_short = "xpw" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexInput = true;
					break;

				case 'p':
					padOutput = true;
					break;

				case 'w':
					{
						char *	endString = NULL;
						char *	startString;

						wrapLines = true;
						if (optarg && *optarg)
						{
							startString = optarg;	
						}
						else
						{
							if ((optind + argo) >= argc)
								break; /* last option, no number present */
							startString = argv[optind + argo];
							if (*startString == '-')
								break; /* next is an option */
						}
						lineSize = strtoul(startString, &endString, 10);
						if (*startString && strlen(endString))
						{
							errorMessage("Invalid line size '%s' specified for -w option.\a\n", startString);
							return(EXIT_FAILURE);
						}
						else
							optind++;
					}
					break;

				check_verbosity_options_short();
				help_option(b64enc);
				getopt_message_displayed(b64enc);
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
	{
		if (hexInput)
		{
			char	withoutSpaces[sizeof(buffer)];
			size_t	used = 0;
			char *	in;
			char *	out;
			int		i;
			size_t	more = read;

			in = buffer;
			out = withoutSpaces;
			while (more > 0)
			{
				for (i = more; i > 0; i--, in++)
				{
					if (isspace(*(in)))
						continue;
					*(out++) = *in;
					used++;
				}
				if (used == sizeof(withoutSpaces))
					break;
				more = fread(buffer, 1, sizeof(withoutSpaces) - used, stdin);
				if (more == 0)
					break;
				in = buffer;
			}
			read = hexadecimalToBinary(withoutSpaces, used, buffer, sizeof(buffer));
			if (read == 0) 
				break;
		}
		
		char		base64[(sizeof(buffer) * 4 / 3) + 1]; /* one more byte for optional end of string */
		size_t		base64Size = binaryToBase64(buffer, read, base64, sizeof(base64) - 1, padOutput);
		
		if (base64Size == 0) break;
		
		uint32_t	toWrite = base64Size;
		char *		out = base64;

		out = wrapOutput(wrapLines, lineSize, &charsOnLine, &toWrite, out);
		if (isAnyError())
			break;

		if ((toWrite > 0) && fwrite(out, toWrite, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			break;
		}
		if (wrapLines)
		{
			charsOnLine += toWrite;
		}
	}
	if (!isAnyError() && wrapLines && (fwrite("\n", 1, 1, stdout) != 1)) /* append newline */
		setError(WRITE_FAILED);
	
	if (isAnyError()) 
	{
		if (isError(WRITE_FAILED))
		{
			errorMessage("Write to STDOUT failed.\a\n");
		}
		else if (isError(INV_HEX_DATA))
		{
			errorMessage("Invalid hexadecimal data value encountered on STDIN.\a\n");
		}
		else if (isError(INV_HEX_SIZE))
		{
			errorMessage("Invalid hexadecimal data size encountered on STDIN.\a\n");
		}
		else if (isError(INV_B64_ENC_SIZE))
		{
			errorMessage("Invalid data size encountered on STDIN.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

// 'hexdec' function - decode hexadecimal presentation of data from STDIN to STDOUT

int hexdec_main(int argc, char** argv, int argo)
{
	char				buffer[80];
	size_t				read = 0;

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
				help_option(hexdec);
				getopt_message_displayed(hexdec);
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
	{
		char	withoutSpaces[sizeof(buffer)];
		size_t	used = 0;
		char *	in;
		char *	out;
		int		i;
		size_t	more = read;

		in = buffer;
		out = withoutSpaces;
		while (more > 0)
		{
			for (i = more; i > 0; i--, in++)
			{
				if (isspace(*(in)))
					continue;
				*(out++) = *in;
				used++;
			}
			if (used == sizeof(withoutSpaces))
				break;
			more = fread(buffer, 1, sizeof(withoutSpaces) - used, stdin);
			if (more == 0)
				break;
			in = buffer;
		}
		read = hexadecimalToBinary(withoutSpaces, used, buffer, sizeof(buffer));
		if (read == 0 || isAnyError()) 
			break;

		if (fwrite(buffer, read, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			break;
		}
	}
		
	if (isAnyError()) 
	{
		if (isError(WRITE_FAILED))
		{
			errorMessage("Write to STDOUT failed.\a\n");
		}
		else if (isError(INV_HEX_DATA))
		{
			errorMessage("Invalid hexadecimal data value encountered on STDIN.\a\n");
		}
		else if (isError(INV_HEX_SIZE))
		{
			errorMessage("Invalid hexadecimal data size encountered on STDIN.\a\n");
		}
		else if (isError(INV_B64_ENC_SIZE))
		{
			errorMessage("Invalid data size encountered on STDIN.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

// 'hexenc' function - encode binary data from STDIN to its hexadecimal presentation on STDOUT

int hexenc_main(int argc, char** argv, int argo)
{
	bool				wrapLines = false;
	uint32_t			lineSize = 80;
	uint32_t			charsOnLine = 0;
	char				buffer[120];
	size_t				read = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "wrap-lines", optional_argument, NULL, 'w' },
		};
		char *			options_short = "w" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'w':
					{
						char *	endString = NULL;
						char *	startString;

						wrapLines = true;
						if (optarg && *optarg)
						{
							startString = optarg;	
						}
						else
						{
							if ((optind + argo) >= argc)
								break; /* last option, no number present */
							startString = argv[optind + argo];
							if (*startString == '-')
								break; /* next is an option */
						}
						lineSize = strtoul(startString, &endString, 10);
						if (*startString && strlen(endString))
						{
							errorMessage("Invalid line size '%s' specified for -w option.\a\n", startString);
							return(EXIT_FAILURE);
						}
						else
							optind++;
					}
					break;

				check_verbosity_options_short();
				help_option(hexenc);
				getopt_message_displayed(hexenc);
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
	{
		char		output[(sizeof(buffer) * 2) + 1]; /* one more byte for optional end of string */
		size_t		outputSize = binaryToHexadecimal(buffer, read, output, sizeof(output) - 1);
		
		if (outputSize == 0) break;
		
		uint32_t	toWrite = outputSize;
		char *		out = output;

		out = wrapOutput(wrapLines, lineSize, &charsOnLine, &toWrite, out);
		if (isAnyError())
			break;
		
		if ((toWrite > 0) && fwrite(out, toWrite, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			break;
		}
		if (wrapLines)
		{
			charsOnLine += toWrite;
		}
	}
	if (!isAnyError() && wrapLines && (fwrite("\n", 1, 1, stdout) != 1)) /* append newline */
		setError(WRITE_FAILED);
	
	if (isAnyError()) 
	{
		if (isError(WRITE_FAILED))
		{
			errorMessage("Write to STDOUT failed.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

// 'user_password' function - compute the password hash for export files with a user-specified password

int user_password_main(int argc, char** argv, int argo)
{
	bool				hexOutput = false;
	char *				password = NULL;
	char				hash[MAX_DIGEST_SIZE];
	uint32_t			hashLen = sizeof(hash);
	char				hex[(sizeof(hash) * 2) + 1];
	size_t				hexLen;
	char *				out;
	size_t				outLen;

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
					break;

				check_verbosity_options_short();
				help_option(user_password);
				getopt_message_displayed(user_password);
				invalid_option(opt);
			}
		}
		if (optind >= (argc - argo))
		{
			errorMessage("Missing password on command line.\a\n");
			usageScreen_user_password();
			return EXIT_FAILURE;
		}
		else
			password = argv[optind + argo];
	}
	else
	{
		errorMessage("Missing password on command line.\a\n");
		usageScreen_user_password();
		return EXIT_FAILURE;
	}

	resetError();

	if ((hashLen = Digest(password, strlen(password), hash, hashLen)) == 0)
	{
		errorMessage("Error computing digest value.\a\n");
		return EXIT_FAILURE;
	}

	hexLen = binaryToHexadecimal((char *) hash, hashLen, hex, sizeof(hex));
	hex[hexLen] = 0;
	verboseMessage("user password converted to key 0x%s\n", hex);
	if (hexOutput)
	{
		outLen = hexLen;
		out = hex;
	}
	else
	{
		outLen = hashLen;
		out = (char *) hash;
	}
	if (fwrite(out, outLen, 1, stdout) != 1)
	{
		errorMessage("Write to STDOUT failed.\a\n");
	}

	return EXIT_SUCCESS;
}

// 'device_password' function - compute the password hash from the specified device properties

int device_password_main(int argc, char** argv, int argo)
{
	bool				hexOutput = false;
	char				hash[MAX_DIGEST_SIZE];
	size_t				hashLen = sizeof(hash);
	char				hex[(sizeof(hash) * 2) + 1];
	char *				out;
	size_t				outLen;
	char *				serial = NULL;
	char *				maca = NULL;
	char *				wlanKey = NULL;
	char *				tr069Passphrase = NULL;

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
					break;

				check_verbosity_options_short();
				help_option(user_password);
				getopt_message_displayed(user_password);
				invalid_option(opt);
			}
		}
		if (optind >= (argc - argo))
		{
			errorMessage("Missing password on command line.\a\n");
			return EXIT_FAILURE;
		}
		else
		{
			int		i = optind + argo;
			int		index = 0;

			char *	*arguments[] = {
				&serial,
				&maca,
				&wlanKey,
				&tr069Passphrase,
				NULL
			};

			while (argv[i])
			{
				*(arguments[index++]) = argv[i++];
				if (!arguments[index])
					break;
			}
			if (!maca)
			{
				errorMessage("At least two arguments (serial and maca) are required.\a\n");
				usageScreen_device_password();
				return EXIT_FAILURE;
			}
		}
	}
	else
	{
		errorMessage("Missing arguments on command line.\a\n");
		usageScreen_device_password();
		return EXIT_FAILURE;
	}

	resetError();

	if (keyFromProperties(hash, &hashLen, serial, maca, wlanKey, tr069Passphrase))
	{
		if (hexOutput)
		{
			outLen = binaryToHexadecimal((char *) hash, hashLen, hex, sizeof(hex));
			out = hex;
		}
		else
		{
			outLen = hashLen;
			out = (char *) hash;
		}
		if (fwrite(out, outLen, 1, stdout) != 1)
		{
			errorMessage("Write to STDOUT failed.\a\n");
		}
	}
	else
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

// 'password_from_device' function - compute the password hash from the current device properties

int password_from_device_main(int argc, char** argv, int argo)
{
	bool				hexOutput = false;
	bool				forExport = false;
	char				hash[MAX_DIGEST_SIZE];
	size_t				hashLen = sizeof(hash);
	char				hex[(sizeof(hash) * 2) + 1];
	char *				out;
	size_t				outLen;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-output", no_argument, NULL, 'x' },
			{ "for-export", no_argument, NULL, 'e' },
		};
		char *			options_short = "xe" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexOutput = true;
					break;

				case 'e':
					forExport = true;
					break;

				check_verbosity_options_short();
				help_option(user_password);
				getopt_message_displayed(user_password);
				invalid_option(opt);
			}
		}
	}

	resetError();

	keyFromDevice(hash, &hashLen, forExport);
	
	if (!isAnyError())
	{
		if (hexOutput)
		{
			outLen = binaryToHexadecimal((char *) hash, hashLen, hex, sizeof(hex));
			out = hex;
		}
		else
		{
			outLen = hashLen;
			out = (char *) hash;
		}
		if (fwrite(out, outLen, 1, stdout) != 1)
		{
			errorMessage("Write to STDOUT failed.\a\n");
		}
	}
	
	return EXIT_SUCCESS;
}

// 'decode_secret' function - decode the specified secret value (in Base32 encoding) perties

int decode_secret_main(int argc, char** argv, int argo)
{
	bool				hexOutput = false;
	char *				out;
	size_t				outLen;
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
					break;

				check_verbosity_options_short();
				help_option(user_password);
				getopt_message_displayed(user_password);
				invalid_option(opt);
			}
		}
		if (optind >= (argc - argo))
		{
			errorMessage("Missing password on command line.\a\n");
			return EXIT_FAILURE;
		}
		else
		{
			int		i = optind + argo;
			int		index = 0;

			char *	*arguments[] = {
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
				errorMessage("Exactly two arguments (base32 encrypted value and hexadecimal key) are required.\a\n");
				usageScreen_decode_secret();
				return EXIT_FAILURE;
			}
		}
	}
	else
	{
		errorMessage("Exactly two arguments (base32 encrypted value and hexadecimal key) are required.\a\n");
		usageScreen_decode_secret();
		return EXIT_FAILURE;
	}

	resetError();

	CipherSizes();

	size_t			secretBufSize = base32ToBinary(secret, (size_t) -1, NULL, 0);
	size_t			keyBufSize = hexadecimalToBinary(key, (size_t) -1, NULL, 0);
	size_t			secretSize = 0;
	size_t			decryptedSize = 0;
	size_t			keySize = 0;
	size_t			dataLen = 0;
	bool			isString = false;

	char *			secretBuffer = (char *) malloc(secretBufSize + cipher_blockSize);
	char *			decryptedBuffer = (char *) malloc(secretBufSize + cipher_blockSize);
	char *			keyBuffer = (char *) malloc(cipher_keyLen);
	char *			hexBuffer = NULL;

	if (!secretBuffer || !decryptedBuffer || !keyBuffer)
	{
		errorMessage("Memory allocation error.\a\n");
		return EXIT_FAILURE;
	}

	memset(secretBuffer, 0, secretBufSize + cipher_blockSize);
	memset(decryptedBuffer, 0, secretBufSize + cipher_blockSize);
	memset(keyBuffer, 0, cipher_keyLen);
	
	resetError();
	secretSize = base32ToBinary(secret, (size_t) -1, (char *) secretBuffer, secretBufSize);
	keySize = hexadecimalToBinary(key, (size_t) -1, (char *) keyBuffer, keyBufSize);

	if (isAnyError())
	{
		errorMessage("The specified arguments contain invalid data.\a\n");
		return EXIT_FAILURE; /* buffers are freed on exit by the run-time */
	}

	if (keySize != 16)
	{
		errorMessage("The specified key has a wrong size.\a\n");
		return EXIT_FAILURE;
	}

	CipherContext 		*ctx = CipherInit(NULL, keyBuffer, secretBuffer);
	
	if (!(secretSize % 16))
		secretSize++;
	if (CipherUpdate(ctx, decryptedBuffer, &decryptedSize, secretBuffer + cipher_ivLen, secretSize - cipher_ivLen))
	{
		if (!DigestCheckValue(decryptedBuffer, decryptedSize, &out, &dataLen, &isString))
		{	
			setError(INVALID_KEY);
			errorMessage("The specified password is wrong.\a\n");
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
				errorMessage("Error allocating memory.\a\n");
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
			errorMessage("Write to STDOUT failed.\a\n");
	}

	decryptedBuffer = clearMemory(decryptedBuffer, secretBufSize, true);
	hexBuffer = clearMemory(hexBuffer, dataLen * 2, true);

	return EXIT_SUCCESS;
}

// 'decode_secrets' function - decode all secret values from STDIN content and copy it with replaced values to STDOUT

int decode_secrets_main(int argc, char** argv, int argo)
{
	char 				hash[MAX_DIGEST_SIZE];
	size_t				hashLen = sizeof(hash);
	char *				serial = NULL;
	char *				maca = NULL;
	char *				wlanKey = NULL;
	char *				tr069Passphrase = NULL;

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
				help_option(user_password);
				getopt_message_displayed(user_password);
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
				&wlanKey,
				&tr069Passphrase,
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

	char			key[cipher_keyLen];

	memset(key, 0, cipher_keyLen);

	if (!serial) /* use device properties from running system */
	{
		if (!keyFromDevice(hash, &hashLen, false))
			return EXIT_FAILURE;
		memcpy(key, hash, cipher_ivLen);
	}
	else if (!maca) /* single argument - assume it's a hexadecimal key already */
	{
		hexadecimalToBinary(serial, strlen(serial), key, cipher_keyLen);
		if (isAnyError())
			return EXIT_FAILURE;
	}
	else if (!wlanKey) /* serial and maca - use an export key from device */
	{
		errorMessage("To use the properties of another device, you have to specify at least three\nvalues ('%s', '%s' and '%s').\a\n", URLADER_SERIAL_NAME, URLADER_MACA_NAME, URLADER_WLANKEY_NAME);
		return EXIT_FAILURE;
	}
	else
	{
		if (!keyFromProperties(hash, &hashLen, serial, maca, wlanKey, tr069Passphrase))
			return EXIT_FAILURE;
		memcpy(key, hash, cipher_ivLen);
	}

	memoryBuffer_t	*inputFile = memoryBufferReadFile(stdin, 8 * 1024);
	
	if (!inputFile)
	{
		if (!isAnyError()) /* empty input file */
			return EXIT_SUCCESS;	
		errorMessage("Error reading STDIN to memory.\a\n");
		return EXIT_FAILURE;
	}

	memoryBufferProcessFile(&inputFile, 0, key, stdout);

	inputFile = memoryBufferFreeChain(inputFile);

	return !isAnyError();
}

// 'decode_export' function - decode all secret values from the export file on STDIN and copy it with replaced values to STDOUT

int decode_export_main(int argc, char** argv, int argo)
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
				help_option(user_password);
				getopt_message_displayed(user_password);
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

	char			key[cipher_keyLen];

	memset(key, 0, cipher_keyLen);

	if (!serial) /* use device properties from running system */
	{
		if (!keyFromDevice(hash, &hashLen, true))
			return EXIT_FAILURE;
		memcpy(key, hash, cipher_ivLen);
	}
	else if (!maca) /* single argument - assume it's a user-defined password */
	{
		hashLen = Digest(serial, strlen(serial), hash, hashLen);
		if (isAnyError())
			return EXIT_FAILURE;
		memcpy(key, hash, cipher_ivLen);
	}
	else
	{
		if (!keyFromProperties(hash, &hashLen, serial, maca, NULL, NULL))
			return EXIT_FAILURE;
		memcpy(key, hash, cipher_ivLen);
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
		memset(key + cipher_ivLen, 0, cipher_keyLen - cipher_ivLen);
		if (passwordIsCorrect)
		{
			char 		hex[(MAX_DIGEST_SIZE * 2) + 1];
			size_t		hexLen = binaryToHexadecimal(key, cipher_keyLen - cipher_ivLen, hex, (MAX_DIGEST_SIZE * 2) + 1);

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

	return !isAnyError();
}

// main entry point for each call

int main(int argc, char** argv)
{
	commandEntry_t *	command = commands;
	int					argumentCount = argc;
	char **				arguments = argv;
	int					argumentOffset = 0;
	char * 				fname;
	char * 				ename;
	char 				enameLong[PATH_MAX];
	
	if (readlink("/proc/self/exe", enameLong, PATH_MAX) == -1)
	{
		errorMessage("Unable to get executable name from procfs.\a\n");
		exit(EXIT_FAILURE);
	}
	if (argumentCount == 0)
	{
		errorMessage("Unable to get invocation name from arguments.\a\n");
		exit(EXIT_FAILURE);
	}
	ename = basename(strdup(enameLong));
	fname = basename(strdup(arguments[0]));
	
	if (strcmp(ename, fname))
	{
		argumentOffset = 0;		
	}
	else if (argumentCount > 1)
	{
		fname = arguments[1];
		argumentOffset = 1;
	}
	else
	{
		usageScreen();
		exit(EXIT_FAILURE);
	}

	while (command->name)
	{
		if (strcmp(fname, command->name))
		{
			command++;
			continue;
		}
		arguments[0] = ename;
		int exitCode = (*command->ep)(argumentCount, arguments, argumentOffset);
		if (exitCode == EXIT_SUCCESS)
		{
			if (isatty(1))
				fprintf(stdout, "\n");
		}
		EVP_cleanup();
		exit(exitCode);
	}

	if (!command->name)
	{
		errorMessage("Unknown function '%s' for '%s' binary.\a\n\n", fname, enameLong);
		usageScreen();
	}

	exit(EXIT_FAILURE);
}
