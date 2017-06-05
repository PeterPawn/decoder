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

#define FUNCTIONS_C

#include "common.h"

// callable functions table

commandEntry_t **	commandsTable[] = {
	&b32dec_command,
	&b32enc_command,
	&b64dec_command,
	&b64enc_command,
	&hexdec_command,
	&hexenc_command,
	&userpw_command,
	&devpw_command,
	&pwfrdev_command,
	&decsngl_command,
	&decfile_command,
	&decexp_command,

#ifdef FREETZ_PACKAGE_DECRYPT_FRITZOS_CFG
	&decfos_command,
#endif

	NULL
};

commandEntry_t *	getCommandEntry(int index)
{
	if (!commandsTable[index]) return NULL;
	return *(commandsTable[index]);
};