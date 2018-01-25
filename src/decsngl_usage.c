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

void 	decsngl_usage(const bool help, UNUSED const bool version)
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
		"This program takes a Base32 encoded value and a hexadecimal key and tries to decrypt the value.\n"
		"If decryption was possible, the clear-text data will be written to STDOUT.\n"
	);

	showFormatHeader(out);
	addSpace();
	addOption("options");
	addSpace();
	endOptions();
	addSpace();
	addArgument("cipher-text");
	addSpace();
	addArgument("key");
	showFormatEnd(out);

	showOptionsHeader("options");
	addOptionsEntry("-x, --hex-output", "output data as a hexadecimal string", 0);
	addOptionsEntryVerbose();
	addOptionsEntryQuiet();
	addOptionsEntryStrict();
	addOptionsEntryHelp();
	addOptionsEntryVersion();
	showOptionsEnd(out);

	fprintf(out,
		"\nThe %s parameter must be a Base32 encoded value (using AVM's 'digits', but without the\n"
		"leading dollar-signs); its size is verified and has to be a multiple of eight.\n",
		showUndl("cipher-text")
	);

	fprintf(out,
		"\nThe %s argument is a hexadecimal string for the decryption key to use and it has to contain 64 or\n"
		"32 characters; values with 32 characters are padded with zeros to a 256-bit key.\n",
		showUndl("key")
	);

	fprintf(out,
		"\nIf the decoded data doesn't look like a C-string (that means, the last byte isn't NUL), the decoded\n"
		"data will be output as a hexadecimal string, prefixed with '0x' to mark this format. If hexadecimal\n"
		"output was requested with option '--hex-output' (or '-x'), this marker is omitted. There's no way\n"
		"to distinguish between the forced output as hexadecimal string and a value, which really starts\n"
		"with '0x'. If hexadecimal output is used, any NUL character isn't removed from the end of the data.\n"
	);

	showUsageFinalize(out, help, version);
}

char *	decsngl_shortdesc(void)
{
	return "decrypt Base32 encoded data using a specified AES key";
}
