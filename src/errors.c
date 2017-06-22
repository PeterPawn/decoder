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
	"invalid data size",
	"warning treated as error",
	"error reading environment value",
	"invalid buffer size specified",
	"conflicting options found",
};

//// error messages ////
EXPORTED	char *			errorAccessingEnvironment = "Error %u (%s) accessing alternative environment path '%s'.\n";
EXPORTED	char *			errorDecryptFileData = "Unable to decrypt file data.\n";
EXPORTED	char *			errorDecryptionFailed =  "Decryption failed with the specified arguments.\n";
EXPORTED	char *			errorDeviceProperties = "To use the properties of another device, you have to specify at least three values ('%s', '%s' and '%s').\n";
EXPORTED	char *			errorDigestComputation = "Error computing digest value.\n";
EXPORTED	char *			errorExecutableName = "Unable to get executable name from procfs.\n";
EXPORTED	char *			errorInvalidArgumentData = "The specified arguments contain invalid data.\n";
EXPORTED	char *			errorInvalidDataSize = "Invalid data size encountered on STDIN.\n";
EXPORTED	char *			errorInvalidFirstStageLength = "Invalid length of data (%u) in the '%s' entry. Expected value is 104.\n";
EXPORTED	char *			errorInvalidFunction = "Unknown function '%s' for '%s' binary.\n";
EXPORTED	char *			errorInvalidHexSize = "Invalid hexadecimal data size encountered on STDIN.\n";
EXPORTED	char *			errorInvalidHexValue = "Invalid hexadecimal data value encountered on STDIN.\n";
EXPORTED	char *			errorInvalidKeyValue = "The specified key value '%s' is invalid, it contains wrong characters.\n";
EXPORTED	char *			errorInvalidValue = "Invalid data value encountered on STDIN.\n";
EXPORTED	char *			errorInvalidWidth = "Invalid line size '%s' specified for %s option.\n";
EXPORTED	char *			errorInvocationName = "Unable to get invocation name from arguments.\n";
EXPORTED	char *			errorMissingArguments = "Missing arguments on command line.\n";
EXPORTED	char *			errorMissingSerialMac = "At least two arguments (serial and maca) are required.\n";
EXPORTED	char *			errorNoMemory = "Memory allocation error.\n";
EXPORTED	char *			errorNoPasswordEntry = "Unable to find the password entry in the provided file.\nIs this really an export file?\n";
EXPORTED	char *			errorPasswordMissing = "Missing password on command line.\n";
EXPORTED	char *			errorReadToMemory = "Error reading data into memory.\n";
EXPORTED	char *			errorUnexpectedError = "Unexpected error %d (%s) encountered.\n";
EXPORTED	char *			errorWriteFailed = "Write to STDOUT failed.\n";
EXPORTED	char *			errorWrongArgumentsCount = "Exactly two arguments (Base32 encrypted value and hexadecimal key) are required.\n";
EXPORTED	char *			errorWrongHexKeyLength = "The specified key value '%s' has a wrong length, 32 hexadecimal digits are the expected value.\n";
EXPORTED	char *			errorWrongKeySize = "The specified key has a wrong size.\n";
EXPORTED	char *			errorWrongPassword = "The specified password is wrong.\n";
EXPORTED	char *			errorMissingDeviceProperty = "The device property '%s' is not set.\n";
EXPORTED	char *			errorOpeningEnvironment = "Error opening environment file '%s'.\n";
EXPORTED	char *			errorReadingEnvironment = "Error reading environment file '%s'.\n";
EXPORTED	char *			errorInvalidOption = "Invalid option '%s' specified.\n";
EXPORTED	char *			errorMissingOptionValue = "Missing value after option '%s'.\n";
EXPORTED	char *			errorInvalidOrAmbiguousOption = "The specified option '%s' is ambiguous or unknown.\n";
EXPORTED	char *			errorReadFromTTY = "STDIN is connected to a terminal device, execution aborted. If it's your intention to enter data from terminal, specify the '--tty' option while calling.\n";
EXPORTED	char *			errorNoReadFromTTY = "STDIN is connected to a terminal device, execution aborted.\n";
EXPORTED	char *			errorWrongMACAddress = "The specified MAC address '%s' has a wrong format.\n";
EXPORTED	char *			errorInvalidBufferSize = "The specified buffer size value '%s' is invalid.\n";
EXPORTED	char *			errorConflictingOptions = "Conflicting options found.\n";
EXPORTED	char *			errorEmptyInputFile = "There's no input data present.\n";
//// end ////

// functions

char *	getErrorText(decoder_error_t err)
{
	return __decoder_error_text__[err];	
}
