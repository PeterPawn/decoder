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

void 	decexp_usage(const bool help, UNUSED const bool version)
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
	addArgument("password");
	addAlternative();
	addArgument("serial");
	addSpace();
	addArgument("maca");
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
	addOptionsEntry("-c, --checksum", "re-compute (and replace) the checksum for the provided export file, after the cipher-text values were replaced with the corresponding clear-text", 0);
	addOptionsEntry("-l, --low-memory", "do not try to consolidate input data into a single buffer", 0);
	addOptionsEntry("-b, --block-size " __undl("size"), "read input data in blocks of the specified " __undl("size"), 8);
	addOptionsEntryVerbose();
	addOptionsEntryQuiet();
	addOptionsEntryStrict();
	addOptionsEntryHelp();
	addOptionsEntryVersion();
	showOptionsEnd(out);

	fprintf(out,
		"\nThe %s has to be a saved-settings file, which was exported from FRITZ!OS.\n",
		showUndl("input-file")
	);

	fprintf(out,
		"\nThese files use a two-stage encryption, where a random value will be chosen and encrypted with the\n"
		"key for the first stage. Then it will be stored in an entry with the name 'Password' in the file\n"
		"header. All further data inside the file will be encrypted with this random key. The first key's\n"
		"value is derived from device properties (see %s for details), if an export file\n"
		"is created without an user-specified %s. Otherwise this %s will be used to compute the\n"
		"first key and you must specify the same %s to decrypt the file.\n",
		showBold(DECODER_CONFIG_KEYS_CURRENT_DEVICE_NAME),
		showUndl("password"), showUndl("password"), showUndl("password")
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
		"\nIf the key value for the second stage can not be decrypted, further processing will be aborted.\n"
	);

	fprintf(out,
		"\nAll input data will be read from STDIN; you can provide it using shell redirections. If STDIN is\n"
		"connected to a terminal device and the '--tty' option (or '-t') wasn't specified, the last value\n"
		"from command line (following %s or %s and %s values) is checked, whether it's the name\n"
		"of a readable file. In this case the specified %s will be used as source of input data.\n",
		showUndl("password"), showUndl("serial"), showUndl("maca"), showUndl("input-file")
	);

	fprintf(out,
		"\nIf the '--tty' option wasn't specified, STDIN is a terminal device and the last argument isn't\n"
		"the name of a readable file, then the execution will be aborted.\n"
	);

	fprintf(out,
		"\nThe encrypted value of the 'Password' field in the header will remain unchanged. Its value isn't\n"
		"usable for other cases and as long as it's kept intact, the file may be later imported again by a\n"
		"FRITZ!OS device (with the right properties or with a password), even if all encrypted values are\n"
		"replaced by their corresponding cleartext. Only the CRC32 checksum at the end of the file has to be\n"
		"re-computed.\n"
	);

	fprintf(out,
		"\nIf you want to import the created output file into a FRITZ!OS device, the program can set a valid\n"
		"checksum at the end of the file, if you've specified the option '--checksum' (or '-c'). This option\n"
		"is mutually exclusive with the '--low-memory' option.\n"
	);

	fprintf(out,
		"\nThe option '--low-memory' (or '-l') may be used, if your system has not enough free memory to hold\n"
		"the input data (from really huge files) twice in memory for a short time.\n"
	);

	fprintf(out,
		"\nThe option '--block-size' (or '-b') to specify the input block size should seldom be necessary. If\n"
		"you specify it, you can set a buffer size between 32 bytes and 16 Mbytes. Any valid number may be\n"
		"followed by a 'K' or 'M' (upper or lower case) to use a factor of 2**10 or 2**20 with the given\n"
		"number. This disables at the same time all further tries to consolidate the input data in a single\n"
		"buffer (as if the option '--low-memory' was used).\n"
	);

	showUsageFinalize(out, help, version);
}

char *	decexp_shortdesc(void)
{
	return "decrypt a FRITZ!OS export file";
}
