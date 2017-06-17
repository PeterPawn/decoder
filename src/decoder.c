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
#include "decoder_usage.c"

// main entry point for each call

int main(int argc, char** argv)
{
	int					argumentCount = argc;
	char **				arguments = argv;
	int					argumentOffset = 0;
	char * 				fname = NULL;
	char * 				ename = NULL;
	char 				enameLong[PATH_MAX+1];
	size_t				linkSize;
	
	if ((linkSize = readlink("/proc/self/exe", enameLong, PATH_MAX)) == (size_t) -1)
	{
		errorMessage(errorExecutableName);
		exit(EXIT_FAILURE);
	}
	enameLong[PATH_MAX] = 0;
	enameLong[linkSize] = 0;
	if (argumentCount == 0)
	{
		errorMessage(errorInvocationName);
		exit(EXIT_FAILURE);
	}
	ename = basename(strdup(enameLong));
	fname = basename(strdup(arguments[0]));
	
	if (strcmp(ename, fname))
	{
		argumentOffset = 0;		
	}
	else if (argumentCount > 1 && *(arguments[1]) != '-')
	{
		fname = arguments[1];
		argumentOffset = 1;
	}
	else
	{
		setAppletName(fname);
		if (argc > 1)
		{
			int			opt;
			int			optIndex = 0;

			static struct option options_long[] = {
				{ "help", no_argument, NULL, 'h' },
				{ "version", no_argument, NULL, 'V' },
				options_long_end,
			};
			char *		options_short = ":hV";

			while ((opt = getopt_long(argc, argv, options_short, options_long, &optIndex)) != -1)
			{
				switch (opt)
				{
					case 'h':
						main_usage(true, false);
#if DEBUG == 1
						fprintf(stdout, "====> applet list - only visible on DEBUG builds <====\n");
						int	i = 0;
						commandEntry_t	*current = getCommandEntry(i);

						while (current)
						{
							char * 		*name = *(current->names);

							fprintf(stdout, "ep=0x%08llx, usage=0x%08llx, usesCrypto=%u, finalNewlineOnTTY=%u, name=", \
								(long long unsigned int) current->ep, (long long unsigned int) current->usage, \
								current->usesCrypto, current->finalNewlineOnTTY);

							while (*name)
							{
								fprintf(stdout, "%s ", *name);
								name++;
							}
							fprintf(stdout, "\n");
							current = getCommandEntry(++i);
						}
#endif
						return EXIT_SUCCESS;

					case 'V':
						main_usage(false, true);
						return EXIT_SUCCESS;

					getopt_invalid_option();
					invalid_option(opt);
				}
			}
		}

		main_usage(false, false);

		exit(EXIT_FAILURE);
	}

	int					i=0;
	commandEntry_t *	current = getCommandEntry(i);

	while (current)
	{
		char * *		name = *(current->names);

		while (*name)
		{
			if (!strcmp(fname, *name))
			{
				if (current->usesCrypto) EncryptionInit();
				arguments[0] = ename;
				opterr = 0;
				setAppletName(*name);
				int exitCode = (*current->ep)(argumentCount, arguments, argumentOffset, current);
				if (exitCode == EXIT_SUCCESS)
				{
					if (current->finalNewlineOnTTY && isatty(1) && !isAnyError())
						fprintf(stdout, "\n");
				}
				if (current->usesCrypto) EVP_cleanup();
				exit(exitCode);
			}
			name++;
		}
		current = getCommandEntry(++i);
	}

	if (!current)
	{
		errorMessage(errorInvalidFunction, fname, enameLong);
		main_usage(false, false);
	}

	exit(EXIT_FAILURE);
}
