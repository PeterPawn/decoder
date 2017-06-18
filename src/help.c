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

#define	HELP_C

#include "common.h"

// common code to show help screens

static	char *			appletNameMask = "\n%s, version " DECODER_MAIN_VERSION "\n\nThis program is a part of the project from https://github.com/PeterPawn/decode_passwords.\n";

static	char *			__ansiBold = "\033[1m";
static	char *			__ansiUndl = "\033[4m";
static	char *			__ansiRset = "\033[0m";
static	char *			optionStart = "[ ";
static	char *			optionEnd = " ]";
static	char *			optionsEnd = "[ -- ]";
static	char *			space = " ";
static	char *			alternative = " | ";

EXPORTED	char *		*ansiBold = &__ansiBold;
EXPORTED	char *		*ansiUndl = &__ansiUndl;
EXPORTED	char *		*ansiRset = &__ansiRset;

static	showOption_t	*options = NULL;
static	char *			commandLine = NULL;
static	char *			emptyString = "";
static	size_t			indent = 0;
#define	INDENT			4

EXPORTED	void	showUsageHeader(FILE * out, const bool help, UNUSED const bool version)
{
	showVersion(out, help);
	fprintf(out, "\n");
	showLicense(out);
}

EXPORTED	void	showUsageFinalize(FILE * out, UNUSED const bool help, UNUSED const bool version)
{
//	fprintf(out, "\nhelp=%u, version=%u\n", help, version);
	fprintf(out, "\n");
}

EXPORTED	void 	showVersion(FILE * out, UNUSED const bool help)
{
	fprintf(out, appletNameMask, showBold(getAppletName()));
}

EXPORTED	void	showPurposeHeader(FILE * out)
{
	fprintf(out, "\n%s\n\n", showBold("Purpose:"));
}

EXPORTED	void	showFormatHeader(FILE * out)
{
	fprintf(out, "\n%s\n", showBold("Usage:"));
	addIndent();
	fprintf(out, "%s%s", newLine(), showBold(getAppletName()));
	commandLine = malloc(1024);
	if (commandLine)
		*(commandLine) = 0;
}

EXPORTED	void	addArgument(const char * arg)
{
	strcat(commandLine, showUndl(arg));
}

EXPORTED	void	addNormalString(const char * arg)
{
	strcat(commandLine, arg);
}

EXPORTED	void	addOption(const char * option)
{
	startOption();
	strcat(commandLine, showUndl(option));
	endOption();
}

EXPORTED	void	startOption(void)
{
	strcat(commandLine, optionStart);
}

EXPORTED	void	endOption(void)
{
	strcat(commandLine, optionEnd);
}

EXPORTED	void	endOptions(void)
{
	strcat(commandLine, optionsEnd);
}

EXPORTED	void	addSpace(void)
{
	strcat(commandLine, space);
}

EXPORTED	void	addAlternative(void)
{
	strcat(commandLine, alternative);
}

EXPORTED	void	showFormatEnd(FILE * out)
{
	fprintf(out, "%s%s\n", commandLine, *ansiRset);
	free(commandLine);
	commandLine = NULL;
	removeIndent();
}

EXPORTED	void	showOptionsHeader(char * option)
{
	commandLine = malloc(8192);
	if (commandLine)
	{	
		memset(commandLine, 0, 8192);
		strcat(commandLine, "\nSupported ");
		strcat(commandLine, showUndl(option));
		strcat(commandLine, " are:\n\n");
	}
}

EXPORTED	void	addOptionsEntry(char * value, char * description, size_t invisible)
{
	showOption_t *	current = options;
	showOption_t *	previous = options;

	while (current)
	{
		previous = current;
		current = current->next;
	}
	
	showOption_t *	new = malloc(sizeof(showOption_t));
	new->next = NULL;
	new->value = value;
	new->description = description;
	new->invisible = invisible;

	if (previous)
		previous->next = new;
	else
		options = new;
}

EXPORTED	void	addOptionsEntryQuiet(void)
{
	addOptionsEntry("-q, --quiet", "suppress any error messages on STDERR", 0);
}

EXPORTED	void	addOptionsEntryVerbose(void)
{
	addOptionsEntry("-v, --verbose", "show additional information on STDERR", 0);
}

EXPORTED	void	addOptionsEntryHelp(void)
{
	addOptionsEntry("-h, --help", "show this information (on STDOUT) and exit", 0);
}

EXPORTED	void	addOptionsEntryVersion(void)
{
	addOptionsEntry("-V, --version", "show version (on STDOUT) and exit", 0);
}

EXPORTED	void	addOptionsEntryStrict(void)
{
	addOptionsEntry("-s, --strict", "treat warnings as errors and exit", 0);
}

void	buildOptionsDisplay(void)
{
	showOption_t *	current = options;
	size_t			left = 0;
	size_t			lines = 0;
	size_t			right = 0;

	while (current)
	{
		lines++;
		if ((strlen(current->value) - current->invisible) > left)
		{
			left = strlen(current->value) - current->invisible;
		}
		current = current->next;				
	}
	right = DECODER_HELP_WIDTH - left - 3;
	current = options;
	while (current)
	{
		if (strlen(current->description) > right)
		{
			lines += (strlen(current->description) / right);
		}
		current = current->next;
	}
	current = options;
	while (current)
	{
		strcat(commandLine, current->value);
		memset((commandLine + strlen(commandLine)), ' ', (left - strlen(current-> value) + current->invisible));
		strcat(commandLine, " - ");
		if (strlen(current->description) > right)
		{
			char *	next = current->description;
			char *	c;

			while (*next)
			{
				if (strlen(next) > right)
				{
					c = next + right;
					while (!isspace(*c))
						c--;
				}
				else
					c = next + strlen(next);
				strncat(commandLine, next, (c - next));
				if (*c)
				{
					strcat(commandLine, "\n");
					*(commandLine + strlen(commandLine) + left + 3) = 0;
					memset(commandLine + strlen(commandLine), ' ', left + 3);
					next = c + 1;
				}
				else
					next = c;
			}
		}
		else
			strcat(commandLine, current->description);

		strcat(commandLine, "\n");

		showOption_t *	previous = current;

		current = current->next;
		free(previous);
	}
}

EXPORTED	void	showOptionsEnd(FILE * out)
{
	if (!options)
	{
		free(commandLine);
		commandLine = NULL;
		return;
	}
	buildOptionsDisplay();
	fprintf(out, commandLine);
	free(commandLine);
	commandLine = NULL;
}

EXPORTED	char *	showBold(const char * data)
{
	size_t			size = strlen(*ansiBold) + strlen(data) + strlen(*ansiRset) + 1;
	char *			buffer = malloc(size);
	
	if (!buffer)
		return NULL; /* memory exhausted */

	snprintf(buffer, size, "%s%s%s", *ansiBold, data, *ansiRset);

	return buffer;
}

EXPORTED	char *	showUndl(const char * data)
{
	size_t			size = strlen(*ansiUndl) + strlen(data) + strlen(*ansiRset) + 1;
	char *			buffer = malloc(size);
	
	if (!buffer)
		return NULL; /* memory exhausted */

	snprintf(buffer, size, "%s%s%s", *ansiUndl, data, *ansiRset);

	return buffer;
}

EXPORTED	void	addIndent(void)
{
	indent += INDENT;
}

EXPORTED	void	removeIndent(void)
{
	indent = (indent >= INDENT ? indent - INDENT : 0);
}

EXPORTED	char *	getIndent(char * prefix, char * suffix)
{
	if (indent)
	{
		char *		buffer = malloc((prefix ? strlen(prefix) : 0) + indent + (suffix ? strlen(suffix) : 0) + 1);
		char *		current = buffer;

		if (!buffer)
			return NULL;

		if (prefix)
		{
			strcpy(current, prefix);
			current += strlen(prefix);
		}

		memset(current, ' ', indent);
		current += indent;

		if (suffix)
		{
			strcpy(current, suffix);
			current += strlen(suffix);
		}

		*current = 0;

		return buffer;	
	}
	else
		return emptyString;
}

EXPORTED	char *	newLine(void)
{
	return getIndent("\n", NULL);
}
