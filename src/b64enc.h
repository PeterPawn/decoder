/*
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2018, Peter Haemmerlein (peterpawn@yourfritz.de)
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

#ifndef B64ENC_H

#define B64ENC_H

#include "common.h"

// function prototypes

void		b64enc_usage(const bool help, const bool version);
int			b64enc_entry(int argc, char** argv, int argo, commandEntry_t * entry);

#ifndef B64ENC_C

extern commandEntry_t * 	b64enc_command;

#endif

#endif

