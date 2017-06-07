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

#define ENVIRON_C

#include "common.h"

// environment file name

static	char *			environmentFileName = URLADER_ENV_PATH;

// error message masks

static	char *			openErrorMessage = "Error opening ";
static	char *			readErrorMessage = "Error readdng ";
static	char *			fileErrorMessage = "environment file '%s'.\n";
static	char *			errorMessageQuestion = "Are we really running on a FRITZ!OS device?\n";

// get/set environment file name

EXPORTED	void	setEnvironmentPath(char * path)
{
	environmentFileName = strdup(path);
}

EXPORTED	char *	getEnvironmentPath(void)
{
	return strdup(environmentFileName);
}

// read environment functions

void	environErrorCommon(void)
{
	errorMessage(fileErrorMessage, environmentFileName);
	errorMessage(errorMessageQuestion);
}

EXPORTED	memoryBuffer_t *	getEnvironmentFile(void)
{
	FILE *				environment = fopen(environmentFileName, "r");

	if (!environment)
	{
		errorMessage(openErrorMessage);
		environErrorCommon();
		setError(URLADER_ENV_ERR);
		return NULL;	
	}
	
	memoryBuffer_t *	env = memoryBufferReadFile(environment, -1);
	fclose(environment);

	if (!env)
	{
		errorMessage(readErrorMessage);
		environErrorCommon();
		setError(URLADER_ENV_ERR);
		return false;
	}

	if (env->next)
	{
		memoryBuffer_t	*new = memoryBufferConsolidateData(env);
		memoryBufferFreeChain(env);
		env = new;
	}

	size_t				offset = 0;
	
	// replace newlines with end of strings for easier access	

	while (offset < env->used)
	{
		if (*(env->data + offset) == '\n')
			*(env->data + offset) = 0;
		offset++;
	}

	return env;
}

EXPORTED	char *	getEnvironmentValue(memoryBuffer_t * environ, char * name)
{
	memoryBuffer_t *	env = (environ ? environ : getEnvironmentFile());
	
	if (!env)
		return NULL;

	char *		current = env->data;
	char *		value = NULL;

	while (current < (env->data + env->used))
	{
		if (!strncmp(current, name, strlen(name)))
		{
			if (*(current + strlen(name)) == '\t')
			{
				value = strdup(current + strlen(name) + 1);
				break;
			}
		}
		current += strlen(current) + 1;
	}

	if (!environ)
		memoryBufferFreeChain(env);
	
	return value;
}
