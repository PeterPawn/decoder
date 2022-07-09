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

#ifndef FUNCTIONS_H

#define FUNCTIONS_H

#include "common.h"

// data types for internal routing to requested function

typedef struct commandEntry commandEntry_t;

struct commandEntry {
	char * *		*names;
	int 			(*ep)(int argc, char **argv, int argo, commandEntry_t * entry);
	void 			(*usage)(const bool help, const bool version);
	char *			(*short_desc)(void);
	bool			usesCrypto;
	bool			finalNewlineOnTTY;
};

#ifndef FUNCTIONS_C

// function table

extern commandEntry_t *	*commandsTable;

#endif

// function definitions

commandEntry_t *	getCommandEntry(int index);

#endif
