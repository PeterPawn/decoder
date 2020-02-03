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

#ifndef ENVIRON_H

#define ENVIRON_H

#include "common.h"

// FRITZ!OS environment related settings

#define	URLADER_ENV_PATH		DECODER_CONFIG_URLADER_ENVIRONMENT_PATH

// environment file functions

char *				getEnvironmentPath(void);
void				setEnvironmentPath(char * path);

memoryBuffer_t *	getEnvironmentFile(void);
char *				getEnvironmentValue(memoryBuffer_t * environ, char * name);

#endif
