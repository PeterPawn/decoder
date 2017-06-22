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

#define CRC32_C

#include "common.h"

EXPORTED	crcCtx_t *	crcInit(void)
{
	crcCtx_t			*ctx = malloc(sizeof(crcCtx_t));
	
	memset(ctx, 0, sizeof(crcCtx_t));
	
	for (uint32_t i = 0; i < (sizeof(ctx->table) / sizeof(uint32_t)); i++)
	{
		uint32_t		v = i;

		for (uint32_t j = 0; j < 8; j++)
		{
			bool		o = ((v & 1) == 1);

			v >>= 1;
			if (o)
				v ^= CRC_POLYNOM;
		}
		ctx->table[i] = v;
	}

	ctx->value = ~(ctx->value);

	return ctx;
}

EXPORTED	void		crcUpdate(crcCtx_t * ctx, const char * input, const size_t size)
{
	uint8_t				byte;

	if (!ctx)
		return;

	for (size_t i = 0; i < size; i++)
	{
		byte = *(input + i);
		ctx->value = (ctx->value >> 8) ^ ctx->table[(ctx->value & 255) ^ byte];
	}
}

EXPORTED	uint32_t	crcFinal(crcCtx_t * ctx)
{
	uint32_t			value;

	if (!ctx)
		return 0;

	value = ~(ctx->value);

	free(ctx);

	return value;
}
