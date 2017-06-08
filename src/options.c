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

#define OPTIONS_C

#include "common.h"

// statics

//// error messages ////
static	char *			errorInvalidWidth = "Invalid line size '%s' specified for %s option.\n";
static	char *			errorAccessingEnvironment = "Error %u (%s) accessing alternative environment path '%s'.\n";
//// end ////
//// verbose messages ////
static	char *			verboseRedirectStdin = "redirecting STDIN to file '%s'\n";
static	char *			verboseTooMuchArguments = "additional command line argument ignored: '%s'\n";
//// end ////

EXPORTED	bool	setAlternativeEnvironment(char * newEnvironment)
{
	struct stat		st;

	if (stat(newEnvironment, &st))
	{
		int			error = errno;

		errorMessage(errorAccessingEnvironment, errno, strerror(error), newEnvironment);
		return false;
	}
	setEnvironmentPath(newEnvironment);

	return true;
}

EXPORTED	bool	checkLastArgumentIsInputFile(char * name)
{
	struct stat		st;

	if (stat(name, &st))
		return false;

	verboseMessage(verboseRedirectStdin, name);
	freopen(name, "r", stdin);

	return true;
}

EXPORTED	void	warnAboutExtraArguments(char ** argv, int i)
{
	int		index = i;

	while (argv[index])
	{
		verboseMessage(verboseTooMuchArguments, argv[index]);
		index++;
	}
}

static		char	optionString[16];

EXPORTED	char *	optionsString(int option, const char * longOption)
{
	if (longOption)
		snprintf(optionString, sizeof(optionString), "--%s", longOption);
	else
		snprintf(optionString, sizeof(optionString), "-%c", option);
	return optionString;
}

EXPORTED	bool	setLineWidth(char * value, const char * option)
{

	setLineWrap();
	if (value && *value)
	{
		char *		endString = NULL;
		char *		startString = value;
		unsigned long	lineSize = strtoul(startString, &endString, 10);

		if (*startString && strlen(endString))
		{
			errorMessage(errorInvalidWidth, startString, option);
			return(false);
		}
		setOutputLineWidth(lineSize);
	}

	return true;
}
