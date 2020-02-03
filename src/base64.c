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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, please look for the file LICENSE.
 */

#include "base64.h"

// Base64 encoding table

static 	char * UNUSED	base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// convert a Base64 string to a binary buffer

size_t	base64ToBinary(char *base64, size_t base64Size, char *binary, size_t binarySize, bool pad, bool ignoreWhitespace)
{
	size_t			offset = 0;
	size_t			outOffset = 0;
	size_t			b64Size = 0;
	bool			filler = false;
	size_t			bSize = 0;
	int				bits = 0;
	int				value = 0;
	size_t			inSize = 0;

	b64Size = (base64Size == (size_t) -1 ? strlen(base64) : base64Size);

	if (ignoreWhitespace)
	{
		char		*current = base64;

		while (*current)
		{
			if (isspace(*(current++))) /* only whitespace characters (0x20, 0x0A, 0x0D, 0x09, 0x0B, 0x0C) will be ignored here */
				continue;
			inSize++;
		}
	}
	else
		inSize = b64Size;

	bSize = (inSize * 3 / 4);

	if (inSize % 4)
	{
		if (pad)	
			bSize = ((inSize / 4) + 1) * 3;
		else
			returnError(INV_B64_SIZE, 0);
	}

	if (bSize > binarySize)
		returnError(BUF_TOO_SMALL, bSize);

	while (offset < b64Size && outOffset < binarySize)
	{
		value = 0;
		for (int i = 0; i < 4 && offset < b64Size; i++)
		{
			char 	c = *(base64 + offset++);

			while (c && ignoreWhitespace && isspace(c))
			{
				c = *(base64 + offset++);
			}

			if (c >= 'A' && c <= 'Z')
				c = c - 'A';
			else if (c >= 'a' && c <= 'z')
				c = c - 'a' + 26;
			else if (c >= '0' && c <= '9')
				c = c - '0' + 52;
			else if (c == '+')
				c = 62;
			else if (c == '/')
				c = 63;
			else if (c == '=')
			{
				filler = true;
				c = 0;
			}
			else
				returnError(INV_B64_DATA, 0);

			value = (value << 6) + c;
			bits += 6;

			if (bits == 24)
			{
				*(binary + outOffset) = (char) (value >> 16);
				*(binary + outOffset + 1) = (char) ((value >> 8) & 0xFF);
				*(binary + outOffset + 2) = (char) (value & 0xFF);
				bits = 0;
				outOffset += 3;
			}
		}
	}

	if (ignoreWhitespace && offset < b64Size) /* skip over whitespace at end of data */
	{
		while (*(base64 + offset) && isspace(*(base64 + offset)))
			offset++;
	}

	if (filler && offset < b64Size)
	{
		returnError(INV_B64_DATA, 0);
	}

	if (bits > 0)
	{
		if (pad)
		{
			if (bits == 12)
				value = value << 12;
			else if (bits == 18)
				value = value << 6;
			else /* at least two characters are needed for one byte */
				returnError(INV_B64_SIZE, 0);
		}
		else
			returnError(INV_B64_SIZE, 0);

		*(binary + outOffset) = (char) (value >> 16);
		outOffset++;
		*(binary + outOffset) = (char) ((value >> 8) & 0xFF);
		outOffset++;

		if (bits > 12)
		{
			if (pad)
			{
				*(binary + outOffset) = 0;
				outOffset++;
			}
		}
	}
	return outOffset;
}

// convert a binary buffer to a Base64 string

size_t	binaryToBase64(char *binary, size_t binarySize, char *base64, size_t base64Size, bool pad)
{
	size_t			offset = 0;
	size_t			outOffset = 0;
	size_t			bSize = (binarySize * 4 / 3);
	int				bits = 0;
	int				value = 0;

	if (binarySize % 3)
		bSize = ((binarySize / 3 ) + 1) * 4;
	if (bSize > base64Size)
		returnError(BUF_TOO_SMALL, 0);
	while (offset < binarySize && outOffset < base64Size)
	{
		value = 0;
		for (int i = 0; i < 3 && offset < binarySize; i++)
		{
			value = (value << 8) + (*(binary + offset++) & 0xFF);
			bits += 8;
			if (bits == 24)
			{
				*(base64 + outOffset) = base64Table[(value >> 18)];
				*(base64 + outOffset + 1) = base64Table[((value >> 12) % 64)];
				*(base64 + outOffset + 2) = base64Table[((value >> 6) % 64)];
				*(base64 + outOffset + 3) = base64Table[(value % 64)];
				bits = 0;
				outOffset += 4;
			}
		}
	}
	if (bits > 0) /* finalize data */
	{
		value = value << (24 - bits);
		*(base64 + outOffset) = base64Table[(value >> 18)];
		outOffset++;
		*(base64 + outOffset) = base64Table[((value >> 12) % 64)];
		outOffset++;
		if (bits == 16)
		{
			*(base64 + outOffset) = base64Table[((value >> 6) % 64)];
			outOffset++;
			if (pad)
			{
				*(base64 + outOffset) = '=';
				outOffset++;
			}
		}
		else
		{
			if (pad)
			{
				*(base64 + outOffset) = '=';
				outOffset++;
				*(base64 + outOffset) = '=';
				outOffset++;
			}
		}
	}
	return outOffset;
}
