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

void 	decfile_usage(const bool help, UNUSED const bool version)
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
		"This program decrypts all occurrences of encrypted data on STDIN (if decryption is possible) and\n"
		"writes the data to STDOUT, while replacing cipher-text with the corresponding clear-text values.\n"
	);

	showFormatHeader(out);
	addSpace();
	addOption("options");
	addSpace();
	endOptions();
	addSpace();
	startOption();
	addArgument("parameter");
	addSpace();
	startOption();
	addNormalString("...");
	endOption();
	endOption();
	addSpace();
	startOption();
	startOption();
	addNormalString("<");
	endOption();
	addSpace();
	addArgument("input-file");
	endOption();
	showFormatEnd(out);

	showOptionsHeader("options");
	addOptionsEntry("-t, --tty", "don't quit execution, if STDIN is connected to a terminal device", 0);
	addOptionsEntry("-a, --alt-env " __undl("filename"), "use an alternative source for the 'urlader environment'", 8);
	addOptionsEntryVerbose();
	addOptionsEntryQuiet();
	addOptionsEntryHelp();
	addOptionsEntryVersion();
	showOptionsEnd(out);

	fprintf(out,
		"\nThe %s may be any text file, starting from a real TFFS node up to a single line of Base32\n"
		"encoded cipher-text, as long as any encrypted value starts with the four dollar-signs. This can be\n"
		"useful, if you extract some settings from a configuration file with other utilities (e.g.\n"
		"'ar7cfgctl') and want to decode only a single value or a smaller amount of values.\n",
		showUndl("input-file")
	);

	fprintf(out,
		"\nSTDIN has to be redirected or the program will be aborted - it's a protection against accidental\n"
		"calls without proper redirection, otherwise the program would block the caller. If you really want\n"
		"to call it with STDIN connected to a terminal device, specify the '--tty' option for the call.\n\n"
		"If STDIN is connected to a terminal device and the '--tty' option (or '-t') wasn't specified, the\n"
		"last expected value from command line is checked, wether it's the name of a readable file. In this\n"
		"case the specified %s will be used as source of input data.\n",
		showUndl("input-file")
	);

	fprintf(out,
		"\nThe number of %ss depends on the mode of the call. If no %s was specified, the\n"
		"decryption key is built from device properties in the 'urlader environment'. If the program isn't\n"
		"called on a FRITZ!OS device, the 'urlader environment' may be substituted by a text file. Its\n"
		"location can be specified with the '--alt-env' (or '-a') option.\n",
		showUndl("parameter"), showUndl("parameter")
	);

	fprintf(out,
		"\nIf a single %s is used, it has to be a hexadecimal string representing the key to be used\n"
		"for decryption.\n",
		showUndl("parameter")
	);

	fprintf(out,
		"\nIf more than one %s exists, it's an alternative approach to decrypt data from another\n"
		"FRITZ!OS device with knowledge of its properties - this one doesn't need an environment file\n"
		"from somewhere. For this mode, you need at least the '%s', '%s' and '%s' values\n"
		"of the foreign device. If it has a pre-configured CWMP account, you need the value from the field\n"
		"'%s' of the device too.\n",
		showUndl("parameter"), showUndl(URLADER_SERIAL_NAME), showUndl(URLADER_MACA_NAME), 
		showUndl(URLADER_WLANKEY_NAME), showUndl(URLADER_TR069PP_NAME)
	);

	fprintf(out,
		"\nIf you've got a copy of the 'urlader environment' for the device, where your %s was\n"
		"created, you should use this file with the '--alt-env' option instead of specifying three and more\n"
		"values for the call.\n",
		showUndl("input-file")
	);

	showUsageFinalize(out, help, version);
}

char *	decfile_shortdesc(void)
{
	return "decrypt Base32 encoded secrets from an input file";
}
