/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2018, Peter Haemmerlein (peterpawn@yourfritz.de)
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

#ifndef OUTPUT_H

#define OUTPUT_H

// verbosity level definitions

typedef enum {
	VERBOSITY_SILENT,	/* no error messages on STDERR */
	VERBOSITY_NORMAL,	/* show only error messages on STDERR */
	VERBOSITY_STRICT,	/* fail on warnings shown on STDERR */
	VERBOSITY_VERBOSE	/* show extra information on STDERR */
} decoder_verbosity_t;

#ifndef OUTPUT_C

// global verbosity setting

extern	decoder_verbosity_t *			__decoder_verbosity;

extern	char *							verboseFoundProperty;
extern	char *							verboseMissingProperty;
extern	char *							verboseAltEnv;
extern	char *							verboseUsingKey;
extern	char *							verbosePasswordHash;
extern	char *							verboseSerialUsed;
extern	char *							verboseMACUsed;
extern	char *							verboseWLANKeyUsed;
extern	char *							verboseTR069PPUsed;
extern	char *							verboseDeviceKeyHash;
extern	char *							verbosePasswordUsed;
extern	char *							verboseRedirectStdin;
extern	char *							verboseTooMuchArguments;
extern	char *							verboseDecryptionFailed;
extern	char *							verboseFoundCipherText;
extern	char *							verboseDecryptedTo;
extern	char *							verboseDecryptedToHex;
extern	char *							verboseDecryptFailed;
extern	char *							verboseDisplayFailed;
extern	char *							verboseWrongSerialLength;
extern	char *							verboseWrongMACAddress;
extern	char *							verboseWrongWLANKey;
extern	char *							verboseWrongTR069Passphrase;
extern	char *							verboseAltEnvIgnored;
extern	char *							verboseWrapLinesIgnored;
extern	char *							verboseBufferSize;
extern	char *							verboseInputDataConsolidated;
extern	char *							verboseNoConsolidate;
extern	char *							verboseChecksumFound;
extern	char *							verboseChecksumIsValid;
extern	char *							verboseNewChecksum;
extern	char *							verboseOpenedOutputFile;

extern	char *							verboseDebugKey;
extern	char *							verboseDebugBase32;
extern	char *							verboseDebugInput;
extern	char *							verboseDebugIV;
extern	char *							verboseDebugEncData;
extern	char *							verboseDebugDecData;
extern	char *							verboseDebugSize;
extern	char *							verboseDebugIsString;
extern	char *							verboseDebugValue;

#endif

// default values

#define DEFAULT_OUTPUT_LINE_WIDTH		DECODER_CONFIG_WRAP_LINE_SIZE

// helper macros

#define	colAppletName(color,bell)		( fprintf(stderr, "\033[1m\033[3" #color "m%s\033[0m: %s", getAppletName(), (bell ? "\a" : "")) )

#define isVerbose()						(__getVerbosity() == VERBOSITY_VERBOSE)

#define errorMessage(...)				if (__getVerbosity() != VERBOSITY_SILENT) {\
											colAppletName(1,1);\
											fprintf(stderr, ##__VA_ARGS__);\
										}

#define warningMessage(...)				if (__getVerbosity() != VERBOSITY_SILENT) {\
											colAppletName(3,0);\
											fprintf(stderr, ##__VA_ARGS__ );\
										}

#define warningMessageNoApplet(...)		if (__getVerbosity() != VERBOSITY_SILENT) {\
											fprintf(stderr, ##__VA_ARGS__ );\
										}

#define verboseMessage(...)				if (__getVerbosity() == VERBOSITY_VERBOSE) {\
											colAppletName(4,0);\
											fprintf(stderr, ##__VA_ARGS__ );\
										}

#define verboseMessageNoApplet(...)		if (__getVerbosity() == VERBOSITY_VERBOSE) {\
											fprintf(stderr, ##__VA_ARGS__ );\
										}

// function prototypes

char *									wrapOutput(FILE * outFile, size_t *charsOnLine, size_t *toWrite, char *output);
char *									optionsString(int option, const char * longOption);
decoder_verbosity_t						__getVerbosity(void);
void									__setVerbosity(decoder_verbosity_t verbosity);
size_t									getOutputLineWidth(void);
void									setOutputLineWidth(size_t width);
void									setLineWrap(void);
bool									getLineWrap(void);
void									setAppletName(char * name);
char *									getAppletName(void);

#endif
