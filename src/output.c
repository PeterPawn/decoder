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

#define OUTPUT_C

#include "common.h"

// global verbosity settings

static 	decoder_verbosity_t		__decoder_verbosity__ = VERBOSITY_NORMAL;

EXPORTED	char *				verboseFoundProperty = "found device property '%s' with value '%s'\n";
EXPORTED	char *				verboseMissingProperty = "device property '%s' does not exist\n";
EXPORTED	char *				verboseAltEnv = "using alternative environment path '%s'\n";
EXPORTED	char *				verboseUsingKey = "using key 0x%s for decryption\n";
EXPORTED	char *				verbosePasswordHash = "user password converted to key 0x%s\n";
EXPORTED	char *				verboseSerialUsed = "using serial number '%s'\n";
EXPORTED	char *				verboseMACUsed = "using maca value '%s'\n";
EXPORTED	char *				verboseWLANKeyUsed = "using wlan_key value '%s'\n";
EXPORTED	char *				verboseTR069PPUsed = "using tr069_passphrase value '%s'\n";
EXPORTED	char *				verboseDeviceKeyHash = "device key value is 0x%s\n";
EXPORTED	char *				verbosePasswordUsed = "specified password is '%s'\n";
EXPORTED	char *				verboseRedirectStdin = "redirecting STDIN to file '%s'\n";
EXPORTED	char *				verboseTooMuchArguments = "additional command line argument ignored: '%s'\n";
EXPORTED	char *				verboseDecryptionFailed = " -> decryption failed\n";
EXPORTED	char *				verboseFoundCipherText = "found cipher text '%s' -> ";
EXPORTED	char *				verboseDecryptedTo = "decrypted to '%s'\n";
EXPORTED	char *				verboseDecryptedToHex = "decrypted to 0x%s\n";
EXPORTED	char *				verboseDecryptFailed = "\033[33m\033[1mdecrypt failed\033[0m\n";
EXPORTED	char *				verboseDisplayFailed = "error displaying value, but decryption was successful\n";
EXPORTED	char *				verboseWrongSerialLength = "the specified serial number '%s' has a wrong length\n";
EXPORTED	char *				verboseWrongMACAddress = "the specified MAC address '%s' has a wrong format\n";
EXPORTED	char *				verboseWrongWLANKey = "the specified WLAN key '%s' has an unusual length\n";
EXPORTED	char *				verboseWrongTR069Passphrase = "the specified TR-069 passphrase looks unusual\n";
EXPORTED	char *				verboseAltEnvIgnored = "the specification for an alternative environment file was ignored\n";
EXPORTED	char *				verboseWrapLinesIgnored = "output data is written as binary content, line break settings will be ignored\n";
EXPORTED	char *				verboseBufferSize = "input data will be read in blocks of %u bytes\n";
EXPORTED	char *				verboseInputDataConsolidated = "input data consolidated in a single buffer with %lu bytes\n";
EXPORTED	char *				verboseNoConsolidate = "input data consolidation will be skipped\n";
EXPORTED	char *				verboseChecksumFound = "found current checksum '%s'\n";
EXPORTED	char *				verboseChecksumIsValid = "the current checksum is still valid\n";
EXPORTED	char *				verboseNewChecksum = "the new checksum '%s' was written instead of the old one\n";
EXPORTED	char *				verboseOpenedOutputFile = "output file '%s' opened\n";

EXPORTED	char *				verboseDebugKey = "key\t: (%03u) 0x%s\n";
EXPORTED	char *				verboseDebugBase32 = "base32\t: (%03u) %s\n";
EXPORTED	char *				verboseDebugInput = "input\t: (%03u) 0x%s\n";
EXPORTED	char *				verboseDebugIV = "iv\t: (%03u) 0x%s\n";
EXPORTED	char *				verboseDebugEncData = "enc'd\t: (%03u) 0x%s\n";
EXPORTED	char *				verboseDebugDecData = "dec'd\t: (%03u) 0x%s\n";
EXPORTED	char *				verboseDebugSize = "size\t: %u\n";
EXPORTED	char *				verboseDebugIsString = "string\t: %s\n";
EXPORTED	char *				verboseDebugValue = "value\t: (%03u) %s\n";

// static settings with accessor functions

static	char *					appletName = NULL;
static	size_t					outputLineWidth = DEFAULT_OUTPUT_LINE_WIDTH;
static	bool					wrapLines = false;
UNUSED	static	FILE *			outputFile = NULL;
UNUSED	static	memoryBuffer_t 	*outputBuffer = NULL;

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

EXPORTED	char * 	wrapOutput(FILE * outFile, size_t *charsOnLine, size_t *toWrite, char *output)
{
	size_t				remOnLine = outputLineWidth - *charsOnLine;
	char *				out = output;

	if (wrapLines && !output)
	{
		if (fwrite("\n", 1, 1, outFile) != 1) /* append newline */
			returnError(WRITE_FAILED, 0);
		returnError(NOERROR, 0);
	}
	if (wrapLines && (*toWrite > remOnLine)) /* wrap on lineSize */
	{
		if ((remOnLine > 0) && (fwrite(out, remOnLine, 1, outFile) != 1)) /* remaining line */
			returnError(WRITE_FAILED, out);
		out += remOnLine;
		*toWrite -= remOnLine;
		*charsOnLine = 0;
		if (fwrite("\n", 1, 1, outFile) != 1) /* append newline */
			returnError(WRITE_FAILED, 0);
		while (*toWrite > outputLineWidth)
		{
			if (fwrite(out, outputLineWidth, 1, outFile) != 1)
				returnError(WRITE_FAILED, 0);
			*toWrite -= outputLineWidth;
			out += outputLineWidth;
			if (fwrite("\n", 1, 1, outFile) != 1) /* append newline */
				returnError(WRITE_FAILED, 0);
		}
	}

	return out;
}
