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

void 	pwfrdev_usage(const bool help, UNUSED const bool version)
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
		"This program computes the device-specific key used to encrypt values stored in TFFS files.\n"
	);

	showFormatHeader(out);
	addSpace();
	addOption("options");
	showFormatEnd(out);

	showOptionsHeader("options");
	addOptionsEntry("-a, --alt-env " __undl("filename"), "use an alternative source for the 'urlader environment'", 8);
	addOptionsEntry("-e, --for-export", "create a key usable for a settings export file", 0);
	addOptionsEntry("-x, --hex-output", "output data as a hexadecimal string", 0);
	addOptionsEntryVerbose();
	addOptionsEntryQuiet();
	addOptionsEntryStrict();
	addOptionsEntryHelp();
	addOptionsEntryVersion();
	showOptionsEnd(out);

	fprintf(out,
		"\nThe program reads the device properties %s, %s and %s from the procfs on a\n"
		"FRITZ!OS device (or from an alternative source) and computes the key used to encrypt security-\n"
		"related settings in TFFS files. If the device in question has a configured CWMP account (for TR-069\n"
		"communications), the property %s is added to this key.\n",
		showUndl(URLADER_SERIAL_NAME), showUndl(URLADER_MACA_NAME), showUndl(URLADER_WLANKEY_NAME), showUndl(URLADER_TR069PP_NAME)
	);

	fprintf(out,
		"\nThe 'urlader environment' is usually available with a fixed path name on procfs, but while a\n"
		"FRITZ!OS device is started, a copy will be created in '/var/env'. If the program is used outside of\n"
		"FRITZ!OS (or it's supposed to use properties from another device), an alternative %s may be\n"
		"set and used with the '--alt-env' (or '-a') option. The original file format (name and value\n"
		"delimited by a single TAB character, no empty lines in the file) must be respected.\n",
		showUndl("filename")
	);

	fprintf(out,
		"\nThe key used to encrypt export files, if they're created without an user-defined password, differs\n"
		"from the key for internal storage. If you need an export key for a device, you may specify the\n"
		"option '--for-export' (or '-e') to create such one.\n"
	);

	fprintf(out,
		"\nThe computed key will be written to STDOUT; if the option '--hex-output' (or '-x') was specified,\n"
		"output data will be converted to a hexadecimal string. If STDOUT is connected to a terminal device,\n"
		"hexadecimal output is activated automatically. As long as the option for hexadecimal output wasn't\n"
		"selected explicitely, the value will get a prefix '0x' in front of it.\n"
	);

	showUsageFinalize(out, help, version);
}

char *	pwfrdev_shortdesc(void)
{
	return "compute a secret key based on properties of the current device";
}
