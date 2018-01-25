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

#ifndef MEMORY_H

#define MEMORY_H

#include "common.h"

#define	DEFAULT_MEMORY_BUFFER_SIZE		DECODER_CONFIG_MEMORY_BUFFER_SIZE

// memory structure for file buffering

// STDIN to memory structures

typedef struct memoryBuffer {
	struct memoryBuffer	*next;
	struct memoryBuffer	*prev;
	size_t				size;
	size_t				used;
	char				data[];
} memoryBuffer_t;

// function prototypes

void				memoryBufferSetSize(size_t size);

memoryBuffer_t *	memoryBufferNew(size_t size);
memoryBuffer_t *	memoryBufferFreeChain(memoryBuffer_t *start);
memoryBuffer_t *	memoryBufferConsolidateData(memoryBuffer_t *start);

memoryBuffer_t *	memoryBufferReadFile(FILE * file, size_t chunkSize);
size_t				memoryBufferDataSize(memoryBuffer_t *top);
bool				memoryBufferProcessFile(memoryBuffer_t * *buffer, size_t offset, char * key, FILE * out, char * filesKey);

char *				memoryBufferFindString(memoryBuffer_t * *buffer, size_t *offset, char *find, size_t findSize, bool *split);
char *				memoryBufferAdvancePointer(memoryBuffer_t * *buffer, size_t *lastOffset, size_t offset);
char *				memoryBufferSearchValueEnd(memoryBuffer_t * *buffer, size_t *offset, size_t * size, bool *split);

void *				clearMemory(void * buffer, size_t size, bool freeBuffer);

#endif
