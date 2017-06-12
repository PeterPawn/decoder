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

#ifndef USERPW_H

#define USERPW_H

#include "common.h"

// function prototypes

void		userpw_usage(const char * name, const bool help, const bool version);
int			userpw_entry(int argc, char** argv, int argo, commandEntry_t * entry, const char * name);

#ifndef USERPW_C

extern commandEntry_t * 	userpw_command;

#endif

#endif
