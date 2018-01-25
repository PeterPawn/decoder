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

void 	pkpwd_usage(const bool help, UNUSED const bool version)
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
		"This program computes the secret key used to encrypt the file '/var/flash/websrv_ssl_key.pem',\n"
		"which contains the private key for the X.509 certificate used to provide the GUI over a TLS-secured\n"
		"connection.\n"
	);

	showFormatHeader(out);
	addSpace();
	addOption("options");
	addSpace();
	endOptions();
	addSpace();
	addOption(URLADER_MACA_NAME);
	showFormatEnd(out);

	showOptionsHeader("options");
	addOptionsEntry("-a, --alt-env " __undl("filename"), "use an alternative source for the 'urlader environment'", 8);
	addOptionsEntryVerbose();
	addOptionsEntryQuiet();
	addOptionsEntryStrict();
	addOptionsEntryHelp();
	addOptionsEntryVersion();
	showOptionsEnd(out);

	fprintf(out,
		"\nThe program reads the device property %s from the procfs on a FRITZ!OS device (or from an\n"
		"alternative source) and computes the secret key, which protects the private key file for the GUI\n"
		"certificate against simple read-outs and later abuse.\n",
		showUndl(URLADER_MACA_NAME)
	);

	fprintf(out,
		"\nIf you want to get the password for another device, you can specify - at your option - the %s\n"
		"value as argument and no 'urlader environment' will be used.\n",
		showUndl(URLADER_MACA_NAME)
	);

	fprintf(out,
		"\nThe 'urlader environment' is usually available with a fixed path name on procfs, but if the\n"
		"program is used outside of FRITZ!OS, an alternative path may be specified with the '--alt-env'\n"
		"(or '-a') option. This option is only useful, if you do not already specify the value to use as\n"
		"last argument on the command line."
	);

	showUsageFinalize(out, help, version);
}

char *	pkpwd_shortdesc(void)
{
	return "compute the secret key used to encrypt the private key for the box certificate (which is used for the GUI)";
}
