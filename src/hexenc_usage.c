/*
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

void 	hexenc_usage(const bool help, UNUSED const bool version)
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
		"This program converts binary data to hexadecimal strings.\n"
	);

	showFormatHeader(out);
	addSpace();
	addOption("options");
	showFormatEnd(out);

	showOptionsHeader("options");
	addOptionsEntry("-w, --wrap-lines [ " __undl("width") " ]", "enable line breaks (wrap lines) on output data and (opt.) define the maximum width of a line (instead of the default value " STRING(DECODER_CONFIG_WRAP_LINE_SIZE) ")", 8);
	addOptionsEntryVerbose();
	addOptionsEntryQuiet();
	addOptionsEntryStrict();
	addOptionsEntryHelp();
	addOptionsEntryVersion();
	showOptionsEnd(out);

	fprintf(out,
		"\nThe encoded data is written to STDOUT. If STDIN is connected to a terminal device, execution will be\n"
		"aborted.\n"
	);

	fprintf(out,
		"\nYou can force lines with limited width using the '--wrap-lines' (or '-w') option. Optionally you can\n"
		"set your wanted line width too.\n"
	);

	showUsageFinalize(out, help, version);
}

char *	hexenc_shortdesc(void)
{
	return "encode data to their hexadecimal representation";
}
