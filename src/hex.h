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

#ifndef HEX_H

#define HEX_H

#include "common.h"

// function prototypes

size_t	hexadecimalToBinary(char *input, size_t inputSize, char *output, size_t outputSize);
size_t	binaryToHexadecimal(char *input, size_t inputSize, char *output, size_t outputSize);

#endif
