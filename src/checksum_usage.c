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

// display usage help

void 	checksum_usage(const bool help, UNUSED const bool version)
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
		"This program computes the CRC-32 value for a FRITZ!OS export file.\n"
	);

	showFormatHeader(out);
	addSpace();
	addOption("options");
	showFormatEnd(out);

	showOptionsHeader("options");
	addOptionsEntry("-d, --all-data", "compute value over the 'raw' input file, don't handle the different parts of an export file", 0);
	addOptionsEntry("-x, --hex-output", "show the computed value as hexadecimal string on STDOUT, otherwise it's written as decimal string", 0);
	addOptionsEntry("-r, --raw-output", "write the 32 bits of the computed value as binary data to STDOUT (host order)", 0);
	addOptionsEntry("-l, --lsb-output", "write the 32 bits of the computed value as binary data to STDOUT (LSB order)", 0);
	addOptionsEntry("-m, --msb-output", "write the 32 bits of the computed value as binary data to STDOUT (MSB order)", 0);
	addOptionsEntryVerbose();
	addOptionsEntryQuiet();
	addOptionsEntryStrict();
	addOptionsEntryHelp();
	addOptionsEntryVersion();
	showOptionsEnd(out);

	fprintf(out,
		"\nUsually the input data is expected to be an export file from FRITZ!OS. In this case, the CRC-32\n"
		"value is re-computed over the different parts of this export file and replaced at the last line of\n"
		"input data, while the whole file is copied to STDOUT.\n"
	);

	fprintf(out,
		"\nIf you specify any of the output options, the value is still recomputed, but input data will not\n"
		"be copied to STDOUT and only the CRC-32 value will be written to STDOUT using the specified format.\n"
	);

	fprintf(out,
		"\nIf the '--all-data' option (or '-d') was specified, input data will not be handled as export file\n"
		"and the CRC-32 value will be computed over the 'raw content'. Output format options are used to set\n"
		"the format of data on STDOUT, input data will never be copied to STDOUT."
	);

	showUsageFinalize(out, help, version);
}

char *	checksum_shortdesc(void)
{
	return "compute checksum on STDIN data";	
}
