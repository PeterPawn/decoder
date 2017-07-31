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

#ifndef COMMON_H

#define COMMON_H

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
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#ifdef DECODER_CONFIG_USE_LIBNETTLE

#include "crypto_nettle.h"

#else

#include <openssl/evp.h>

#include "crypto_ossl.h"

#endif

#include "config.h"
#include "errors.h"

#include "base32.h"
#include "base64.h"
#include "hex.h"
#include "crc32.h"

#include "functions.h"
#include "memory.h"
#include "output.h"
#include "help.h"
#include "license.h"
#include "options.h"
#include "environ.h"

#include "encryption.h"
#include "exportfile.h"

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
#include "deccb.h"
#include "pkpwd.h"
#include "checksum.h"
#include "decompose.h"

#endif
