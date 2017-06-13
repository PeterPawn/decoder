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

#define OUTPUT_C

#include "common.h"

// global verbosity settings

static 	decoder_verbosity_t		__decoder_verbosity__ = VERBOSITY_NORMAL;

EXPORTED	char *				verboseFoundProperty = "found device property '%s' with value '%s'\n";
EXPORTED	char *				verboseMissingProperty = "device property '%s' does not exist\n";
EXPORTED	char *				verboseAltEnv = "using alternative environment path '%s'\n";
EXPORTED	char *				verboseUsingKey = "using key 0x%s for decryption\n";
EXPORTED	char *				verbosePasswordHash = "user password converted to key 0x%s\n";
EXPORTED	char *				verbosePasswordUsed = "specified password is '%s'\n";
EXPORTED	char *				verboseRedirectStdin = "redirecting STDIN to file '%s'\n";
EXPORTED	char *				verboseTooMuchArguments = "additional command line argument ignored: '%s'\n";
EXPORTED	char *				verboseDecryptionFailed = " -> decryption failed\n";
EXPORTED	char *				verboseFoundCipherText = "found cipher text '%s' -> ";
EXPORTED	char *				verboseDecryptedTo = "decrypted to '%s'\n";
EXPORTED	char *				verboseDecryptedToHex = "decrypted to 0x%s\n";
EXPORTED	char *				verboseDecryptFailed = "decrypt failed\n";
EXPORTED	char *				verboseDisplayFailed = "error displaying value, but decryption was successful\n";
EXPORTED	char *				verboseWrongSerialLength = "the specified serial number '%s' has a wrong length\n";
EXPORTED	char *				verboseWrongMACAddress = "the specified MAC address '%s' has a wrong format\n";
EXPORTED	char *				verboseWrongWLANKey = "the specified WLAN key '%s' has an unusual length\n";
EXPORTED	char *				verboseWrongTR069Passphrase = "the specified TR-069 passphrase looks unusual\n";

// static settings with accessor functions

static	char *					appletName = NULL;
static	size_t					outputLineWidth = DEFAULT_OUTPUT_LINE_WIDTH;
static	bool					wrapLines = false;

// get verbosity level

EXPORTED	decoder_verbosity_t	__getVerbosity(void)
{
	return __decoder_verbosity__;
}

// set verbosity level

EXPORTED	void	__setVerbosity(decoder_verbosity_t verbosity)
{
	__decoder_verbosity__ = verbosity;
}

// get line size

EXPORTED	size_t	getOutputLineWidth(void)
{
	return outputLineWidth;
}

// set line size

EXPORTED	void 	setOutputLineWidth(size_t width)
{
	outputLineWidth = width;
}

// get/set line wrap

EXPORTED	bool	getLineWrap(void)
{
	return wrapLines;
}

EXPORTED	void	setLineWrap(void)
{
	wrapLines = true;
}

// get/set applet name for error messages

EXPORTED	void	setAppletName(char * name)
{
	appletName = name;
}

EXPORTED	char *	getAppletName(void)
{
	return appletName;
}

// output formatting

EXPORTED	char * 				wrapOutput(uint32_t *charsOnLine, uint32_t *toWrite, char *output)
{
	uint32_t					remOnLine = outputLineWidth - *charsOnLine;
	char *						out = output;

	if (wrapLines && !output)
	{
		if (fwrite("\n", 1, 1, stdout) != 1) /* append newline */
			returnError(WRITE_FAILED, 0);
		returnError(NOERROR, 0);		
	}
	if (wrapLines && (*toWrite > remOnLine)) /* wrap on lineSize */
	{
		if ((remOnLine > 0) && (fwrite(out, remOnLine, 1, stdout) != 1)) /* remaining line */
			returnError(WRITE_FAILED, out);
		out += remOnLine;
		*toWrite -= remOnLine;
		*charsOnLine = 0;
		if (fwrite("\n", 1, 1, stdout) != 1) /* append newline */
			returnError(WRITE_FAILED, 0);
		while (*toWrite > outputLineWidth)
		{
			if (fwrite(out, outputLineWidth, 1, stdout) != 1)
				returnError(WRITE_FAILED, 0);
			*toWrite -= outputLineWidth;
			out += outputLineWidth;
			if (fwrite("\n", 1, 1, stdout) != 1) /* append newline */
				returnError(WRITE_FAILED, 0);
		}
	}

	return out;
}
