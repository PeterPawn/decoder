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

// display usage help

void 	deccb_usage(const bool help, UNUSED const bool version)
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
		"This program expects the hexadecimal encoded content of a CRYPTEDBINFILE entry from an export file\n"
		"and tries to decrypt it. If decryption was possible, the clear-text data will be written to STDOUT.\n"
	);

	showFormatHeader(out);
	addSpace();
	addOption("options");
	addSpace();
	endOptions();
	addSpace();
	addNormalString("{ ");
	addArgument("password");
	addAlternative();
	addArgument("serial");
	addSpace();
	addArgument("maca");
	addNormalString(" }");
	showFormatEnd(out);

	showOptionsHeader("options");
	addOptionsEntry("-t, --tty", "don't quit execution, if STDIN is connected to a terminal device", 0);
	addOptionsEntry("-x, --hex-output", "output data as a hexadecimal string", 0);
	addOptionsEntry("-w, --wrap-lines [ " __undl("width") " ]", "enable line breaks (wrap lines) for textual output data and (opt.) define the maximum width of a line (instead of the default value " STRING(DECODER_CONFIG_WRAP_LINE_SIZE) ")", 8);
	addOptionsEntry("-a, --alt-env " __undl("filename"), "use an alternative source for the 'urlader environment'", 8);
	addOptionsEntryVerbose();
	addOptionsEntryQuiet();
	addOptionsEntryHelp();
	addOptionsEntryVersion();
	showOptionsEnd(out);

	fprintf(out,
		"\nThe input data has to be extracted from the body of a CRYPTEDBINFILE entry - that means, the lines\n"
		"(starting with asterisks) around the hexadecimal lines have to be removed.\n"
	);

	fprintf(out,
		"\nIf the export file was created with a %s, this %s must be specified as the only\n"
		"parameter. If no %s was used to export the settings, there are two alternatives to specify\n"
		"the two needed properties of the source device. You can use the second form of parameters with\n"
		"%s and %s set to the correct values or you can use an alternative file for the 'urlader\n"
		"environment' (using the '--alt-env' (or '-a') option) and the program will extract the values there.\n",
		showUndl("password"), showUndl("password"), showUndl("password"),
		showUndl("serial"), showUndl("maca")
	);

	fprintf(out,
		"\nIf a file was created without a password, there are two alternatives to specify the two needed\n"
		"properties of the source device. You can use the second form of parameters with %s and %s set\n"
		"to the correct values or you can use an alternative file for the 'urlader environment' (using the\n"
		"'--alt-env' (or '-a') option from above) and the program will extract the values there.\n"
		"\nAn alternative environment file is ignored, if the first format with %s is used.\n",
		showUndl("serial"), showUndl("maca"), showUndl("password")
	);

	fprintf(out,
		"\nIf the content can not be decrypted (decrypted raw data contain 16 bytes at the end, where we can\n"
		"check a successful decryption), nothing is written to STDOUT. Without any further options, output\n"
		"data will be written in 'raw' format - that means, the decrypted content will be written 'as is'.\n"
		"This could be a problem with binary files and you may specify the '--hex-output' (or '-x') option\n"
		"to output data with hexadecimal encoding. If you use this option, you can force lines with limited\n"
		"width using the '--wrap-lines' (or '-w') option. Optionally you can set your wanted line width too.\n"
	);

	fprintf(out,
		"\nIf the '--tty' option wasn't specified and STDIN is a terminal device, then the execution will be\n"
		"aborted. The last value from command-line will NOT be checked, if it's a valid file name.\n"
	);

	showUsageFinalize(out, help, version);
}

char *	deccb_shortdesc(void)
{
	return "decrypt CRYPTEDBINFILE data from an export file";
}
