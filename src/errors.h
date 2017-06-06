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

#ifndef ERRORS_H

#define ERRORS_H

// named error codes

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
	// invalid length of second stage key entry
	DECODER_ERROR_INV_DATA_SIZE,
} decoder_error_t;

#ifndef ERRORS_C

// glocal error text definitions

extern char *							*__decoder_error_text;

// global error state

extern decoder_error_t *				__decoder_error;

#endif

// function prototypes

char *	getErrorText(decoder_error_t err);

// helper macros

#define setError(err)                   *__decoder_error = DECODER_ERROR_##err

#define resetError()					setError(NOERROR)

#define returnError(err,value)          { setError(err); return (value); }

#define getError()						(*__decoder_error)

#define isAnyError()					(getError() != DECODER_ERROR_NOERROR)

#define isError(err)					(getError() == DECODER_ERROR_##err)

// options check default

#define invalid_option(opt)				default:\
											errorMessage("Invalid option '%c' specified.\a\n", (char) opt);\
											exit(EXIT_FAILURE)

#endif
