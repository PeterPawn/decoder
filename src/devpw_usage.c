/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2019, Peter Haemmerlein (peterpawn@yourfritz.de)
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

void 	devpw_usage(const bool help, UNUSED const bool version)
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
		"This program takes up to four parameters from the command line (each of them stands for a property\n"
		"from the 'urlader environment' of a FRITZ!OS device) and generates a device-specific encryption key\n"
		"from their values.\n"
	);

	showFormatHeader(out);
	addSpace();
	addOption("options");
	addSpace();
	endOptions();
	addSpace();
	addArgument("prop1");
	addSpace();
	addArgument("prop2");
	addSpace();
	startOption();
	addArgument("prop3");
	addSpace();
	startOption();
	addArgument("prop4");
	endOption();
	endOption();
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
		"\nThe following properties are needed to create a valid key:\n"
	);

	fprintf(out,
		"\n%s is the serial number of the device, it consists of 15 characters and can be found at the\n"
		"back of the device, together with other info printed on a sticker. But for a long time this serial\n"
		"number was only printed there and only the latest models contain this value in a place, where it\n"
		"can be read by the firmware. All older models used a character string of 16 zeros here. You have to\n"
		"know, what value your device really uses - but you could try both approaches, if you've problems\n"
		"with a key version. The correct value can be found as %s in a support-data file or in an\n"
		"environment dump.\n",
		showUndl("prop1"), showUndl(URLADER_SERIAL_NAME)
	);

	fprintf(out,
		"\n%s is the value of the %s field from environment and it's used as Ethernet address (MAC) on\n"
		"the LAN side of a FRITZ!OS device. Its format is also well-known (6 groups of two hexadecimal digits\n"
		"(upper-case for A to Z), separated by colons) and the program verifies the format to ensure it's\n"
		"valid.\n",
		showUndl("prop2"), showUndl(URLADER_MACA_NAME)
	);

	fprintf(out,
		"\nThe %s value is the content of the pre-defined WLAN key (%s) from factory-settings and it\n"
		"can be found on the sticker too. It consists of 16 or 20 digits and the program verifies the\n"
		"expected format.\n",
		showUndl("prop3"), showUndl(URLADER_WLANKEY_NAME)
	);

	fprintf(out,
		"\nThe %s value is only used, if the device in question is equipped with a CWMP account in its\n"
		"factory settings. Then the content of the %s entry is added to the computation of a\n"
		"device-specific key.\n",
		showUndl("prop4"), showUndl(URLADER_TR069PP_NAME)
	);

	fprintf(out,
		"\nThe third and fourth arguments (%s and %s) are optional (and the latter\n"
		"value may be missing on a device). If a device-specific key is intended to encrypt settings\n"
		"export files, FRITZ!OS versions will use only the first two properties to compute the key value.\n",
		showUndl(URLADER_WLANKEY_NAME), showUndl(URLADER_TR069PP_NAME)
	);

	fprintf(out,
		"\nThe computed key will be written to STDOUT; if the option '--hex-output' (or '-x') was specified,\n"
		"output data will be converted to a hexadecimal string. If STDOUT is connected to a terminal device,\n"
		"hexadecimal output is activated automatically. As long as the option for hexadecimal output wasn't\n"
		"selected explicitely, the value will get a prefix '0x' in front of it.\n"
	);

	showUsageFinalize(out, help, version);
}

char *	devpw_shortdesc(void)
{
	return "compute a secret key based on specified device properties";
}
