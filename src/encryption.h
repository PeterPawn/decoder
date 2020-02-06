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

#ifndef ENCRYPTION_H

#define ENCRYPTION_H

#include "common.h"

// FRITZ!OS environment related settings

#define	URLADER_SERIAL_NAME		"SerialNumber"
#define	URLADER_MACA_NAME		"maca"
#define	URLADER_WLANKEY_NAME	"wlan_key"
#define	URLADER_TR069PP_NAME	"tr069_passphrase"

#define	EXPORT_PASSWORD_NAME	"\nPassword=$$$$"
#define	EXPORT_PASSWORD2_NAME	"\nPassword2=$$$$"

#define	PRIVKEY_PASSWORD_SIZE	8

// decrytion related functions

void	encryptionInit(void);

bool	digestCheckValue(char *buffer, size_t bufferSize, char * *value, size_t * dataLen, bool * string);

bool	decryptValue(CipherContext * ctx, char * cipherText, size_t cipherTextSize, FILE * out, char * outBuffer, char * key, bool escaped);
bool	decryptFile(char * input, size_t inputSize, FILE * out, char * outBuffer, char * key, bool hexOutput);

bool	keyFromDevice(char * hash, size_t * hashSize, bool forExport);
bool	keyFromProperties(char * hash, size_t * hashSize, char * serial, char * maca, char * wlanKey, char * tr069Passphrase);

bool	privateKeyPassword(char * out, size_t * outLen, char * maca);

bool	checkMACAddress(char * mac);

#endif
