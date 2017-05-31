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
};

typedef enum {
	VERBOSITY_SILENT,
	VERBOSITY_NORMAL,
	VERBOSITY_VERBOSE
} decoder_verbosity_t;

// constant

#define AVM_HASH_SIZE			16
#define AVM_KEY_SIZE			32
#define	AVM_IV_SIZE				16
#define AVM_BLOCK_SIZE			16

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
void	usageScreen_decode_secret(void);

size_t	base32ToBinary(char *base32, size_t base32Size, char *binary, size_t binarySize);
size_t	binaryToBase32(char *binary, size_t binarySize, char *base32, size_t base32Size);
size_t	base64ToBinary(char *base64, size_t base64Size, char *binary, size_t binarySize, bool pad);
size_t	binaryToBase64(char *binary, size_t binarySize, char *base64, size_t base64Size, bool pad);
size_t	hexadecimalToBinary(char *input, size_t inputSize, char *output, size_t outputSize);
size_t	binaryToHexadecimal(char *input, size_t inputSize, char *output, size_t outputSize);
char *	wrapOutput(bool wrapLines, uint32_t lineSize, uint32_t *charsOnLine, uint32_t *toWrite, char *output);

// entry point prototypes

int b32dec_main(int argc, char** argv, int argo);
int b32enc_main(int argc, char** argv, int argo);
int b64dec_main(int argc, char** argv, int argo);
int b64enc_main(int argc, char** argv, int argo);
int hexdec_main(int argc, char** argv, int argo);
int hexenc_main(int argc, char** argv, int argo);
int user_password_main(int argc, char** argv, int argo);
int device_password_main(int argc, char** argv, int argo);
int decode_secret_main(int argc, char** argv, int argo);

#endif

