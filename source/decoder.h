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

#ifndef DECODER_H

#define DECODER_H

typedef int (*entryPoint)(int argc, char **argv, int argo);

typedef struct 
{
	char			*name;
	entryPoint		ep;	
} commandEntry_t;

typedef enum {
	// no error
	DECODER_ERROR_NOERROR = 0,
	// error writing data to file
	DECODER_ERROR_WRITE_FAILED,
	// buffer too small for output data
	DECODER_ERROR_BUF_TOO_SMALL,
	// error allocating memory
	DECODER_ERROR_NO_MEMORY,
	// invalid Base32 character
	DECODER_ERROR_INV_B32_DATA,
	// invalid size of Base32 encoded string
	DECODER_ERROR_INV_B32_SIZE,
	// invalid Base64 character
	DECODER_ERROR_INV_B64_DATA,
	// invalid size of Base64 encoded string
	DECODER_ERROR_INV_B64_SIZE,
	// invalid size of data to encode (must be a multiple of 5) 
	DECODER_ERROR_INV_B32_ENC_SIZE,
	// invalid size of data to encode (must be a multiple of 3 without padding) 
	DECODER_ERROR_INV_B64_ENC_SIZE,
	// invalid hexadecimal characters
	DECODER_ERROR_INV_HEX_DATA,
	// invalid hexadecimal data size
	DECODER_ERROR_INV_HEX_SIZE,
	// invalid decipher key was specified
	DECODER_ERROR_INVALID_KEY,
	// error returned from an OpenSSL cipher function
	DECODER_ERROR_OSSL_CIPHER_ERR,
	// error returned from an OpenSSL digest function
	DECODER_ERROR_OSSL_DIGEST_ERR,
	// error reading STDIN file into memory buffers
	DECODER_ERROR_STDIN_BUFFER_ERR,
	// error opening/reading device environment on a FRITZ!OS device
	DECODER_ERROR_URLADER_ENV_ERR,
	// error decrypting a file with the specified arguments
	DECODER_ERROR_DECRYPT_ERR,
	// STDIN file looks invalid and not like an export file 
	DECODER_ERROR_INVALID_FILE,
} decoder_error_t;

static char *		__decoder_error_text[] = {
	"no error",
	"write failed",
	"buffer too small",
	"no memory",
	"invalid base32 data",
	"invalid base32 data size",
	"invalid base64 data",
	"invalid base64 data size",
	"invalid data size for base32 encode",
	"invalid data size for base64 encode",
	"invalid hexadecimal data",
	"invalid hexadecimal data size",
	"invalid decipher key",
	"OpenSSL cipher function failed",
	"OpenSSL digest function failed",
	"error reading stdin data to memory",
	"unable to open device environment",
	"decryption failed",
	"not an export file",
};

typedef enum {
	VERBOSITY_SILENT,
	VERBOSITY_NORMAL,
	VERBOSITY_VERBOSE
} decoder_verbosity_t;

// STDIN to memory structures

typedef struct memoryBuffer {
	struct memoryBuffer	*next;
	struct memoryBuffer	*prev;
	size_t				size;
	size_t				used;
	char				data[];
} memoryBuffer_t;

memoryBuffer_t *	memoryBufferNew(size_t size);
memoryBuffer_t *	memoryBufferFreeChain(memoryBuffer_t *start);
memoryBuffer_t *	memoryBufferReadFile(FILE * file, size_t chunkSize);
char *				memoryBufferFindString(memoryBuffer_t * *buffer, size_t *offset, char *find, size_t findSize, bool *split);
char *				memoryBufferAdvancePointer(memoryBuffer_t * *buffer, size_t *lastOffset, size_t offset);

// FRITZ!OS environment related settings

#ifdef USE_REAL_PROCFS
#define	URLADER_ENV_PATH		"/proc/sys/urlader/environment"
#else
#define URLADER_ENV_PATH		"/var/env"
#endif
#define URLADER_SERIAL_NAME		"SerialNumber"
#define URLADER_MACA_NAME		"maca"
#define URLADER_WLANKEY_NAME	"wlan_key"
#define URLADER_TR069PP_NAME	"tr069_passphrase"

#define EXPORT_PASSWORD_NAME	"\nPassword=$$$$"

// EVP types 

#define CipherContext			EVP_CIPHER_CTX
#define DigestContext			EVP_MD_CTX
#define CipherType				EVP_aes_256_cbc()
#define DigestType				EVP_md5()
#define MAX_DIGEST_SIZE			EVP_MAX_MD_SIZE

// cipher and digest functions

void			CipherSizes();
CipherContext *	CipherInit(CipherContext * ctx, char * key, char * iv);
CipherContext *	CipherCleanup(CipherContext * ctx);
bool			CipherUpdate(CipherContext * ctx, char *output, size_t *outputSize, char *input, size_t inputSize);

DigestContext *	DigestInit();
DigestContext * DigestCleanup(DigestContext * ctx);
bool			DigestUpdate(DigestContext * ctx, char * buffer, size_t bufferSize);
bool			DigestFinal(DigestContext * ctx, char * output);
size_t			Digest(char *buffer, size_t bufferSize, char *digest, size_t digestSize);
size_t			DigestLength(DigestContext * ctx);
bool			DigestCheckValue(char *buffer, size_t bufferSize, char * *value, size_t * dataLen, bool * string);
bool			DecryptValue(CipherContext * ctx, char * cipherText, size_t valueSize, FILE * out, char * outBuffer, char * key, bool escaped);
bool			keyFromDevice(char * hash, size_t * hashSize, bool forExport);
bool			keyFromProperties(char * hash, size_t * hashSize, char * serial, char * maca, char * wlanKey, char * tr069Passphrase);


// function prototypes

void	usageScreen(void);
void	usageScreen_b32dec(void);
void	usageScreen_b32enc(void);
void	usageScreen_b64dec(void);
void	usageScreen_b64enc(void);
void	usageScreen_hexdec(void);
void	usageScreen_hexenc(void);
void	usageScreen_user_password(void);
void	usageScreen_device_password(void);
void	usageScreen_password_from_device(void);
void	usageScreen_decode_secret(void);
void	usageScreen_decode_secrets(void);
void	usageScreen_decode_export(void);

size_t	base32ToBinary(char *base32, size_t base32Size, char *binary, size_t binarySize);
size_t	binaryToBase32(char *binary, size_t binarySize, char *base32, size_t base32Size);
size_t	base64ToBinary(char *base64, size_t base64Size, char *binary, size_t binarySize, bool pad);
size_t	binaryToBase64(char *binary, size_t binarySize, char *base64, size_t base64Size, bool pad);
size_t	hexadecimalToBinary(char *input, size_t inputSize, char *output, size_t outputSize);
size_t	binaryToHexadecimal(char *input, size_t inputSize, char *output, size_t outputSize);
char *	wrapOutput(bool wrapLines, uint32_t lineSize, uint32_t *charsOnLine, uint32_t *toWrite, char *output);
void *	clearMemory(void * buffer, size_t size, bool freeBuffer);

// entry point 

int b32dec_main(int argc, char** argv, int argo);
int b32enc_main(int argc, char** argv, int argo);
int b64dec_main(int argc, char** argv, int argo);
int b64enc_main(int argc, char** argv, int argo);
int hexdec_main(int argc, char** argv, int argo);
int hexenc_main(int argc, char** argv, int argo);
int user_password_main(int argc, char** argv, int argo);
int device_password_main(int argc, char** argv, int argo);
int password_from_device_main(int argc, char** argv, int argo);
int decode_secret_main(int argc, char** argv, int argo);
int decode_secrets_main(int argc, char** argv, int argo);
int decode_export_main(int argc, char** argv, int argo);

#endif

