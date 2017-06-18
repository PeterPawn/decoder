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

void	main_usage(const bool help, const bool version)
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
		"This program is a 'multi-call' binary. Its function is either selected by the first argument on\n"
		"the command-line or by the name of the (hard or symbolic) link, which is used to call it.\n"
		"\nLet's call these functions 'applets' from now on, like BusyBox does it.\n"
	);

	showFormatHeader(out);
	addSpace();
	addNormalString("{ ");
	addArgument("options");
	addAlternative();
	addArgument("applet");
	addSpace();
	addOption("parameters");
	addNormalString(" }");
	showFormatEnd(out);

	showOptionsHeader("options");
	addOptionsEntryHelp();
	addOptionsEntryVersion();
	showOptionsEnd(out);

	fprintf(out,
		"\nThe %s names are configurable at build time, the available names and their purposes are shown\n"
		"below.\n"
		"\nAny applicable %s rely on the wanted %s and are shown, if you call an %s with the\n"
		"'--help' (or '-h') option.\n",
		showUndl("applet"), showUndl("parameters"), showUndl("applet"), showUndl("applet")
	);

	fprintf(out,
		"\nConfigured %s and their names are:\n",
		showUndl("applets")
	);

	int					i = 0;
	size_t				left = 0;
	size_t				right = 0;
	size_t				leftFill = 0;
	commandEntry_t		*current = getCommandEntry(i);
	char *				*name;
	bool				moreThanOneName = false;

	while (current)
	{
 		name = *(current->names);
		moreThanOneName = false;
			
		while (*name)
		{
			if ((strlen(*name) + (moreThanOneName ? 3 : 0)) > left)
			{
				left = strlen(*name) + (moreThanOneName ? 3 : 0);	
			}
			name++;
			moreThanOneName = true;
		}
		if (strlen((*current->short_desc)()) > right)
			right = strlen((*current->short_desc)());
		current = getCommandEntry(++i);
	}


	leftFill = left;
	fprintf(out, "\napplet");
	leftFill -= 6;
	for (size_t j = 0; j < leftFill; j++)
	{
		fprintf(out, " ");
	}
	fprintf(out, "   purpose\n");
	for (size_t j = 0; j < (left + 3 + right); j++)
		fprintf(out, "=");
	fprintf(out, "\n");

	i = 0;
	current = getCommandEntry(i);

	while (current)
	{
		leftFill = left;
		fprintf(out, "\n");
		name = *(current->names);
		fprintf(out, "%s", showBold(*name));
		leftFill -= strlen(*name);
		name++;
		if (*name)
		{
			fprintf(out, " or");
			leftFill -= 3;
		}
		for (size_t j = 0; j < leftFill; j++)
		{
			fprintf(out, " ");
		}
		fprintf(out, " - ");
		fprintf(out, "%s", (*current->short_desc)());
		while (*name)
		{
			fprintf(out, "\n");
			fprintf(out, "%s", showBold(*name));
			name++;
			if (*name)
				fprintf(out, " or");
		}
		current = getCommandEntry(++i);
		fprintf(out, "\n" );
	}

	showUsageFinalize(out, help, version);
}
