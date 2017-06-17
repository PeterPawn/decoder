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

#include "hex.h"

// hexadecimal translation table

static 	char * UNUSED	hexTable = "0123456789ABCDEF";

// hexadecimal conversion functions

// convert a hexadecimal string to a binary buffer

size_t 	hexadecimalToBinary(char *input, size_t inputSize, char *output, size_t outputSize)
{
	size_t		    offset = 0;
	size_t			outOffset = 0;
	size_t			inSize = (inputSize == (size_t) -1 ? strlen(input) : inputSize);
	int				value = 0;
	bool			high = true;
	
	if ((inSize / 2) > outputSize)
		returnError(BUF_TOO_SMALL, (inSize / 2));
	while (offset < inSize && outOffset < outputSize)
	{
		char		c = *(input + offset++) & 0xFF;
	
		if (c >= 'A' && c <= 'F')
		{
			c = c - 'A' + 10;
		}
		else if (c >= 'a' && c <= 'f')
		{
			c = c - 'a' + 10;
		}
		else if (c >= '0' && c <= '9')
		{
			c -= '0';
		}
		else if (isspace(c))
			continue;
		else
		{
			returnError(INV_HEX_DATA, 0);
		}
		if (high)
		{
			value = c << 4;
			high = false;
		}
		else
		{
			value = value + c;
			high = true;
			*(output + outOffset++) = value;
		}
	}
	if (!high)
		returnError(INV_HEX_SIZE, 0);
	return outOffset;
}

// convert a binary buffer to a hexadecimal string

size_t	binaryToHexadecimal(char *input, size_t inputSize, char *output, size_t outputSize)
{
	size_t			index = 0;

	if ((inputSize * 2) > (outputSize - 1))
		returnError(BUF_TOO_SMALL, (inputSize * 2));
	while ((index < inputSize) && ((index * 2) < (outputSize - 1)))
	{
		*(output + (index * 2)) = hexTable[(((*(input + index)) >> 4) & 0x0F)];
		*(output + (index * 2) + 1) = hexTable[((*(input + index)) & 0x0F)];
		index++;
	}
	return (index * 2);
}
