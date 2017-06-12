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

#ifndef DEVPW_H

#define DEVPW_H

#include "common.h"

// function prototypes

void		devpw_usage(char * name, bool help);
int			devpw_entry(int argc, char** argv, int argo, commandEntry_t * entry, char * name);

#ifndef DEVPW_C

extern commandEntry_t * 	devpw_command;

#endif

#endif
