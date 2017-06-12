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

#ifndef OUTPUT_H

#define OUTPUT_H

// verbosity level definitions

typedef enum {
	VERBOSITY_SILENT,
	VERBOSITY_NORMAL,
	VERBOSITY_VERBOSE
} decoder_verbosity_t;

#ifndef OUTPUT_C

// global verbosity setting

extern decoder_verbosity_t *			__decoder_verbosity;

#endif

// default values

#define DEFAULT_OUTPUT_LINE_WIDTH		DECODER_CONFIG_WRAP_LINE_SIZE

// helper macros

#define verbosity_options_long			{ "verbose", no_argument, NULL, 'v' },\
										{ "quiet", no_argument, NULL, 'q' },\
										{ "help", no_argument, NULL, 'h' },\
										{ "version", no_argument, NULL, 'V' }

#define	options_long_end				{ NULL, 0, NULL, 0 }

#define verbosity_options_short			"qvhV"

#define check_verbosity_options_short()	case 'v':\
											__setVerbosity(VERBOSITY_VERBOSE);\
											break;\
										case 'q':\
											__setVerbosity(VERBOSITY_SILENT);\
											break

#define help_option()					case 'h':\
											__usage(true, false);\
											return EXIT_FAILURE;\
										case 'V':\
											__usage(false, true);\
											return EXIT_FAILURE;\

#define getopt_message_displayed()		case '?':\
											__usage(false, false);\
											return EXIT_FAILURE

#define getopt_argument_missing()		case ':':\
											errorMessage("Missing value after option '%s'.\n", getopt_option_name());\
											return EXIT_FAILURE

#define getopt_option_name()			optionsString((optIndex ? optopt : 0), (optIndex ? NULL : options_long[optIndex].name))

#define isVerbose()						(__getVerbosity() == VERBOSITY_VERBOSE)

#define errorMessage(...)				if (__getVerbosity() != VERBOSITY_SILENT) fprintf(stderr, ##__VA_ARGS__); fprintf(stderr, "\a")

#define verboseMessage(...)				if (__getVerbosity() == VERBOSITY_VERBOSE) fprintf(stderr, ##__VA_ARGS__)

#define __usage(help, version)			((*entry->usage)(name, help, version))

#define getAppletName()					(strdup(name))

// function prototypes

char *									wrapOutput(uint32_t *charsOnLine, uint32_t *toWrite, char *output);
char *									optionsString(int option, const char * longOption);
decoder_verbosity_t						__getVerbosity(void);
void									__setVerbosity(decoder_verbosity_t verbosity);
size_t									getOutputLineWidth(void);
void									setOutputLineWidth(size_t width);
void									setLineWrap(void);
bool									getLineWrap(void);

#endif
