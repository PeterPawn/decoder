/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * vim: set tabstop=4 syntax=c :
 *
 * Copyright (C) 2014-2019, Peter Haemmerlein (peterpawn@yourfritz.de)
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

#define BASE32_C

#include "base32.h"

// Base32 encoding table

static 	char * UNUSED	base32Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456";

// Base32 conversion functions

// convert a Base32 string to a binary buffer

size_t	base32ToBinary(char *base32, size_t base32Size, char *binary, size_t binarySize)
{
	size_t			offset = 0;
	size_t			outOffset = 0;
	size_t			b32Size = (base32Size == (size_t) -1 ? strlen(base32) : base32Size);
	
	if (b32Size % 8)
		returnError(INV_B32_SIZE, 0);
	if ((b32Size * 5 / 8) > binarySize)
		returnError(BUF_TOO_SMALL, (b32Size * 5 / 8));
	while (offset < b32Size && outOffset < binarySize)
	{
		int			bits = 0;
		int			value = 0;

		for (int i = 0; i < 8; i++)
		{
			char 	c = *(base32 + offset + i);
	
			if (c >= 'A' && c <= 'Z')
				c = c - 'A';
			else if (c >= '1' && c <= '6')
				c = c - '1' + 26;
			else
				returnError(INV_B32_DATA, 0);

			value = (value << 5) + c;
			bits += 5;

			if (bits >= 8)
			{
				*(binary + outOffset) = (char) (value >> (bits - 8));
				bits -= 8;
				value = value % (1 << bits);
				outOffset++;
			}
		}
		offset += 8;
	}
	return outOffset;
}

// convert a binary buffer to a Base32 string

size_t	binaryToBase32(char *binary, size_t binarySize, char *base32, size_t base32Size)
{
	size_t			offset = 0;
	size_t			outOffset = 0;

	if (binarySize % 5)
		returnError(INV_B32_ENC_SIZE, 0);
	if ((binarySize * 8 / 5) > base32Size)
		returnError(BUF_TOO_SMALL, 0);
	while (offset < binarySize && outOffset < base32Size)
	{
		int			bits = 0;
		int			value = 0;

		for (int i = 0; i < 5; i++)
		{
			value = (value << 8) + (*(binary + offset + i) & 0xFF);
			bits += 8;
			while (bits >= 5)
			{
				*(base32 + outOffset) = base32Table[(value >> (bits - 5))];
				bits -= 5;
				value = value % (1 << bits);
				outOffset++;
			}
		}
		offset += 5;
	}
	return outOffset;
}
