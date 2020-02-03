/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2020, Peter Haemmerlein (peterpawn@yourfritz.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, please look for the file LICENSE.
 */

// display usage help

void 	decompose_usage(const bool help, UNUSED const bool version)
{
	FILE *	out = (help || version ? stdout : stderr);

	showUsageHeader(out, help, version);

	if (version)
	{
		fprintf(out, "\n");
		return;
	}

	showPurposeHeader(out);
	fprintf(out,
		"This program splits a single export file into the settings files contained therein.\n"
	);

	showFormatHeader(out);
	addSpace();
	addOption("options");
	showFormatEnd(out);

	showOptionsHeader("options");
	addOptionsEntry("-o, --output " __undl("directory"), "specifies the " __undl("directory") ", where the files will be stored; this option is mandatory (and therefore not really an option)", 8);
	addOptionsEntry("-d, --dictionary", "create a dictionary file and store header data separately", 0);
	addOptionsEntryVerbose();
	addOptionsEntryQuiet();
	addOptionsEntryStrict();
	addOptionsEntryHelp();
	addOptionsEntryVersion();
	showOptionsEnd(out);

	fprintf(out,
		"\nThe file with exported FRITZ!OS settings is expected on STDIN. Execution will be aborted, if STDIN\n"
		"is a terminal device.\n"
	);

	fprintf(out,
		"\nThe output directory has to exist. Any data file therein may be overwritten without further warning.\n"
		"\nIf you want to create a new file to be imported from the decomposed export file, you may specify the\n"
		"'--dictionary' (or '-d') option to store the original header lines and a list of all contained files,\n"
		"preserving their order.\n"
	);

	showUsageFinalize(out, help, version);
}

char *	decompose_shortdesc(void)
{
	return "split an export file, with optional decryption";	
}
