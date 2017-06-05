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

#define ERRORS_C

#include "common.h"

// global error state

static decoder_error_t		__decoder_error__ = DECODER_ERROR_NOERROR;
UNUSED decoder_error_t *	__decoder_error = &__decoder_error__;

// global error descriptions

static char *				__decoder_error_text__[] = {
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

// functions

char *	getErrorText(decoder_error_t err)
{
	return __decoder_error_text__[err];	
}
