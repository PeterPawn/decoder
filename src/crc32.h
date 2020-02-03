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

#ifndef CRC32_H

#define CRC32_H

#include "common.h"

#define	CRC_POLYNOM			0xEDB88320	

// CRC context

typedef struct crcCtx {
	uint32_t	value;
	uint32_t	table[256];
} crcCtx_t;

// function prototypes

crcCtx_t *		crcInit(void);
void			crcUpdate(crcCtx_t * ctx, const char * input, const size_t size);
uint32_t		crcFinal(crcCtx_t * ctx);

#endif
