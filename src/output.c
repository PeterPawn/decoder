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

#define OUTPUT_C

#include "common.h"

// global verbosity settings

static decoder_verbosity_t		__decoder_verbosity__ = VERBOSITY_NORMAL;

// get verbosity level

EXPORTED	decoder_verbosity_t	__getVerbosity(void)
{
	return __decoder_verbosity__;
}

// set verbosity level

EXPORTED	void 				__setVerbosity(decoder_verbosity_t verbosity)
{
	__decoder_verbosity__ = verbosity;
}

// output formatting

EXPORTED	char * 				wrapOutput(bool wrapLines, uint32_t lineSize, uint32_t *charsOnLine, uint32_t *toWrite, char *output)
{
	uint32_t					remOnLine = lineSize - *charsOnLine;
	char *						out = output;

	if (wrapLines && (*toWrite > remOnLine)) /* wrap on lineSize */
	{
		if ((remOnLine > 0) && (fwrite(out, remOnLine, 1, stdout) != 1)) /* remaining line */
			returnError(WRITE_FAILED, out);
		out += remOnLine;
		*toWrite -= remOnLine;
		*charsOnLine = 0;
		if (fwrite("\n", 1, 1, stdout) != 1) /* append newline */
			returnError(WRITE_FAILED, 0);
		while (*toWrite > lineSize)
		{
			if (fwrite(out, lineSize, 1, stdout) != 1)
				returnError(WRITE_FAILED, 0);
			*toWrite -= lineSize;
			out += lineSize;
			if (fwrite("\n", 1, 1, stdout) != 1) /* append newline */
				returnError(WRITE_FAILED, 0);
		}
	}
	return out;
}

static		char				optionString[16];

EXPORTED	char *				optionsString(int option, const char * longOption)
{
	if (longOption)
		snprintf(optionString, sizeof(optionString), "--%s", longOption);
	else
		snprintf(optionString, sizeof(optionString), "-%c", option);
	return optionString;
}
