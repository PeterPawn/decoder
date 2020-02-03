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

#define ENVIRON_C

#include "common.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wformat-security"

// environment file name

static	char *			environmentFileName = URLADER_ENV_PATH;

// get/set environment file name

EXPORTED	void	setEnvironmentPath(char * path)
{
	environmentFileName = strdup(path);
}

EXPORTED	char *	getEnvironmentPath(void)
{
	return strdup(environmentFileName);
}

EXPORTED	memoryBuffer_t *	getEnvironmentFile(void)
{
	FILE *				environment = fopen(environmentFileName, "r");

	if (!environment)
	{
		errorMessage(errorOpeningEnvironment, environmentFileName);
		setError(URLADER_ENV_ERR);
		return NULL;
	}

	// 2 KByte are usually enough room for an environment ... if it's
	// really too small, consolidation takes place later

	memoryBuffer_t *	env = memoryBufferReadFile(environment, 2048);

	fclose(environment);

	if (!env)
	{
		errorMessage(errorReadingEnvironment, environmentFileName);
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

#pragma GCC diagnostic pop
