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

#ifndef STDLIBS_H

#define STDLIBS_H

#define _GNU_SOURCE

#define EXPORTED                        __attribute__((__visibility__("default"), used, externally_visible))
#define UNUSED                          __attribute__((unused))

#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <getopt.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <openssl/evp.h>

#include "errors.h"

#include "base32.h"
#include "base64.h"
#include "hex.h"

#include "functions.h"
#include "memory.h"
#include "output.h"

#include "crypto.h"
#include "encryption.h"

#include "b32dec.h"
#include "b32enc.h"
#include "b64dec.h"
#include "b64enc.h"
#include "hexdec.h"
#include "hexenc.h"
#include "userpw.h"
#include "devpw.h"
#include "pwfrdev.h"
#include "decsngl.h"
#include "decfile.h"
#include "decexp.h"

#endif
