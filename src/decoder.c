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

#define DECODER_C

#include "decoder.h"

// display usage help

void	main_usage(bool help)
{
	errorMessage("decoder for AVM's cipher implementation\n");
	if (help)
		errorMessage("help option used");
}

// main entry point for each call

int main(int argc, char** argv)
{
	int					argumentCount = argc;
	char **				arguments = argv;
	int					argumentOffset = 0;
	char * 				fname;
	char * 				ename;
	char 				enameLong[PATH_MAX];
	
	if (readlink("/proc/self/exe", enameLong, PATH_MAX) == -1)
	{
		errorMessage("Unable to get executable name from procfs.\a\n");
		exit(EXIT_FAILURE);
	}
	if (argumentCount == 0)
	{
		errorMessage("Unable to get invocation name from arguments.\a\n");
		exit(EXIT_FAILURE);
	}
	ename = basename(strdup(enameLong));
	fname = basename(strdup(arguments[0]));
	
	if (strcmp(ename, fname))
	{
		argumentOffset = 0;		
	}
	else if (argumentCount > 1)
	{
		fname = arguments[1];
		argumentOffset = 1;
	}
	else
	{
		main_usage(false);
		exit(EXIT_FAILURE);
	}

	int					i=0;
	commandEntry_t *	current = getCommandEntry(i);

	while (current)
	{
		if (strcmp(fname, current->name))
		{
			current = getCommandEntry(++i);
			continue;
		}
		if (i >= 6) EncryptionInit();
		arguments[0] = ename;
		int exitCode = (*current->ep)(argumentCount, arguments, argumentOffset, current);
		if (exitCode == EXIT_SUCCESS)
		{
			if (isatty(1))
				fprintf(stdout, "\n");
		}
		EVP_cleanup();
		exit(exitCode);
	}

	if (!current)
	{
		errorMessage("Unknown function '%s' for '%s' binary.\a\n\n", fname, enameLong);
		main_usage(false);
	}

	exit(EXIT_FAILURE);
}
