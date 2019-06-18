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

#ifndef HELP_H

#define HELP_H

typedef	struct	showOption	{
	struct showOption *	next;
	char *				value;
	size_t				invisible;
	char *				description;
} 	showOption_t;

#define	DECODER_MAIN_VERSION			"0.3"
#define DECODER_HELP_WIDTH				99

#define	__undl(string)					"\033[4m" string "\033[0m"
#define	STRING(number)					__STRING(number)

void									showUsageHeader(FILE * out, const bool help, const bool version);
void									showUsageFinalize(FILE * out, const bool help, const bool version);
void									showVersion(FILE * out, const bool help);
void									showPurposeHeader(FILE * out);
void									showFormatHeader(FILE * out);
void									addArgument(const char * arg);
void									addNormalString(const char * arg);
void									addOption(const char * option);
void									startOption(void);
void									endOption(void);
void									endOptions(void);
void									addSpace(void);
void									addAlternative(void);
void									showFormatEnd(FILE * out);
void									showOptionsHeader(char * option);
void									addOptionsEntry(char * value, char * description, size_t invisible);
void									addOptionsEntryVerbose(void);
void									addOptionsEntryHelp(void);
void									addOptionsEntryVersion(void);
void									addOptionsEntryVerbose(void);
void									addOptionsEntryQuiet(void);
void									addOptionsEntryStrict(void);
void									buildOptionsDisplay(void);
void									showOptionsEnd(FILE * out);
char *									showBold(const char * data);
char *									showUndl(const char * data);
void									addIndent(void);
void									removeIndent(void);
char *									getIndent(char * prefix, char * suffix);
char *									newLine(void);

#endif
