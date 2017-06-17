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

#define MEMORY_C

#include "common.h"

// memory buffer oriented functions

static	size_t		memoryBufferSize = DEFAULT_MEMORY_BUFFER_SIZE;

EXPORTED	void	memoryBufferSetSize(size_t newSize)
{
	memoryBufferSize = newSize;
}

// create a new buffer

EXPORTED	memoryBuffer_t *	memoryBufferNew(size_t size)
{
	memoryBuffer_t	*new = (memoryBuffer_t *) malloc(size);
	if (new)
	{
		memset(new, 0, size);
		new->size = size;
	}
	return new;
}

// free all buffers in a chain

EXPORTED	memoryBuffer_t *	memoryBufferFreeChain(memoryBuffer_t *start)
{
	memoryBuffer_t	*curr = start;
	
	while (curr)
	{
		memoryBuffer_t	*next = curr->next;
		
		memset(curr, 0, curr->size);
		free(curr);
		curr = next;
	}
	return NULL;
}

// read a file into one or more memory buffers and return the head element

EXPORTED	memoryBuffer_t *	memoryBufferReadFile(FILE * file, size_t chunkSize)
{
	size_t			allocSize;
	memoryBuffer_t	*top = NULL;
	memoryBuffer_t	*curr = NULL;
	char *			data;
	size_t			read;
	size_t			toRead;

	allocSize = (chunkSize != (size_t) -1 ? chunkSize : memoryBufferSize);
	allocSize += sizeof(memoryBuffer_t);
	top = memoryBufferNew(allocSize);
	curr = top;
	data = top->data;
	toRead = curr->size - curr->used - sizeof(memoryBuffer_t);
	while ((read = fread(data, 1, toRead, file)) > 0)
	{
		curr->used += read;
		data += read;
		if ((read == toRead) && (curr->used == curr->size - sizeof(memoryBuffer_t))) /* buffer is full */
		{
			memoryBuffer_t	*next = memoryBufferNew(allocSize);
			if (!next)
			{
				setError(STDIN_BUFFER_ERR);
				top = memoryBufferFreeChain(top);
				break;
			}
			curr->next = next;
			next->prev = curr;
			curr = next;
			data = curr->data;
			toRead = curr->size - sizeof(memoryBuffer_t);
		}
	}
	if (!top->used)
	{	
		setError(NOERROR);
		free(top);
		top = NULL;
	}
	return top;
}

// compute the size of data stored in a memory buffer chain

EXPORTED 	size_t	memoryBufferDataSize(memoryBuffer_t * top)
{
	size_t			size = 0;
	memoryBuffer_t	*current = top;

	while (current)
	{
		size += current->used;
		current = current->next;
	}

	return size;
}

// consolidate the data from the specified buffer chain into a single buffer
// even if data is already in a single buffer, a copy will be created

EXPORTED	memoryBuffer_t *	memoryBufferConsolidateData(memoryBuffer_t * start)
{
	size_t			size = memoryBufferDataSize(start);
	memoryBuffer_t	*current = start;
	memoryBuffer_t	*new = memoryBufferNew(size);

	if (!new) return NULL;

	char *			out = new->data;

	while (current)
	{
		memcpy(out, current->data, current->used);
		out += current->used;
		current = current->next;
	}
	new->used = size;

	return new;
}

// find a string in the buffer chain, handles crossing buffer borders

EXPORTED	char *	memoryBufferFindString(memoryBuffer_t * *buffer, size_t *offset, char *find, size_t findSize, bool *split)
{
	memoryBuffer_t	*current = *buffer;
	
	while (current)
	{
		char *		chk = current->data + *offset;
		size_t		remaining = current->used - *offset;

		while (remaining > 0)
		{
			if (remaining < findSize)
			{
				if (strncmp(chk, find, remaining) == 0)
				{
					if (current->next)
					{
						if (strncmp(current->data, find + remaining, findSize - remaining) == 0) /* match across buffers */
						{
							*buffer = current;
							*offset = (chk - current->data);
							*split = true;
						}
					}
					else /* partial match at end of data */
						return NULL;
				}
				chk++;
				remaining--;
				if (remaining == 0) /* next buffer, if any */
				{
					if (current->next)
					{
						current = current->next;
						remaining = current->used;
						chk = current->data;
					}
					else /* no match */
						return NULL;
				}				
			}
			else
			{
				if (strncmp(chk, find, findSize) == 0)
				{
					*buffer = current;
					*offset = (chk - current->data);
					*split = false;
					return chk;
				}	
				chk++;
				remaining--;
			}
		}
	}
	return NULL;	
}

// advance a pointer value, following the buffer chain, if a buffer is exhausted

EXPORTED	char *	memoryBufferAdvancePointer(memoryBuffer_t * *buffer, size_t *lastOffset, size_t offset)
{
	memoryBuffer_t	*current = *buffer;
	size_t			advance = offset;
	
	while (current)
	{
		if ((current->used - *lastOffset) < advance) /* next buffer */
		{
			if (current->next)
			{
				advance -= (current->used - *lastOffset);
				current = current->next;
				*buffer = current;
				*lastOffset = 0;
				continue;
			}
		}
		*lastOffset += advance;
		return (current->data + *lastOffset);
	}
	return NULL;	
}

// find the first non-Base32 character from the specified pointer

EXPORTED	char *	memoryBufferSearchValueEnd(memoryBuffer_t * *buffer, size_t *offset, size_t * size, bool *split)
{
	memoryBuffer_t	*current = *buffer;
	size_t  		curOffset = *offset;
	size_t			count = 0;
	char *			cur = current->data + curOffset;

	*split = false;	
	while (current)
	{
		if (current->used <= curOffset) /* next buffer */
		{
			if (current->next)
			{
				current = current->next;
				*buffer = current;
				*offset = 0;
				cur = current->data;
				*split = true;
			}
		}
		if ((*cur >= 'A' && *cur <= 'Z') || (*cur >= '1' && *cur <= '6'))
		{
			count++;
			cur++;
			curOffset++;
		}
		else /* character outside of our Base32 set found */
		{
			*buffer = current;
			*offset = curOffset;
			*size = count;
			return cur;
		}
	}
	return NULL;	
}

// scan memory buffer and replace occurrences of encrypted data while writing data to an output file 

EXPORTED	bool	memoryBufferProcessFile(memoryBuffer_t * *buffer, size_t offset, char * key, FILE * out)
{
	CipherContext 		*ctx = CipherInit(NULL, CipherTypeValue, NULL, NULL, false);
	memoryBuffer_t 		*currentBuffer = *buffer;
	size_t				currentOffset = offset;
	
	while (currentBuffer)
	{
		memoryBuffer_t	*found = currentBuffer;
		size_t			foundOffset = currentOffset;
		bool			split = false;
		char *			cipherTextStart;
	
		if ((cipherTextStart = memoryBufferFindString(&found, &foundOffset, "$$$$", 4, &split)) != NULL) /* encrypted data exists */
		{
			while (currentBuffer && (currentBuffer != found)) /* output data crosses at least one buffer boundary */
			{
				if (fwrite(currentBuffer->data + currentOffset, currentBuffer->used - currentOffset, 1, out) != 1)
				{
					setError(WRITE_FAILED);
					currentBuffer = NULL;
					break;
				}
				currentBuffer = currentBuffer->next;
				currentOffset = 0;
			}
			if (currentBuffer)
			{
				if (fwrite(currentBuffer->data + currentOffset, foundOffset - currentOffset, 1, out) != 1)
				{
					setError(WRITE_FAILED);
					currentBuffer = NULL;
					break;
				}
				currentOffset = foundOffset;
			}
			cipherTextStart = memoryBufferAdvancePointer(&currentBuffer, &currentOffset, 4);
			found = currentBuffer;
			foundOffset = currentOffset;

			size_t		valueSize;
			char *		cipherTextEnd = memoryBufferSearchValueEnd(&found, &foundOffset, &valueSize, &split);
			if (cipherTextEnd)
			{
				char *	copy;
				char *	cipherText = (char *) malloc(valueSize + 1);
			
				memset(cipherText, 0, valueSize + 1);
				copy = cipherText;
				while (currentBuffer && (currentBuffer != found))
				{
					memcpy(copy, currentBuffer->data + currentOffset, currentBuffer->used - currentOffset);
					copy += (currentBuffer->used - currentOffset);
					currentBuffer = currentBuffer->next;
					currentOffset = 0;
				}
				memcpy(copy, currentBuffer->data + currentOffset, foundOffset - currentOffset);
				*(copy + valueSize) = 0;

				if (!DecryptValue(ctx, cipherText, valueSize, out, NULL, key, true)) /* unable to decrypt, write data as is */
				{
					verboseMessage(verboseDecryptionFailed);
					if (fwrite("$$$$", 4, 1, out) == 1)
					{
						if (fwrite(currentBuffer->data + currentOffset, currentBuffer->used - currentOffset, 1, out) != 1)
						{
							setError(WRITE_FAILED);
							currentBuffer = NULL;
						}
					}
					else
					{
						setError(WRITE_FAILED);
						currentBuffer = NULL;
					}
				}
				else
				{
					currentBuffer = found;
					currentOffset = foundOffset;
				}
				free(cipherText);
			}
		}
		else /* no more encrypted data, write remaining buffers */
		{
			while (currentBuffer)
			{
				if (fwrite(currentBuffer->data + currentOffset, currentBuffer->used - currentOffset, 1, out) != 1)
				{
					setError(WRITE_FAILED);
					currentBuffer = NULL;
					break;
				}
				currentBuffer = currentBuffer->next;
				currentOffset = 0;
			}
		}
	}

	ctx = CipherCleanup(ctx);

	return !isAnyError();

}

// free memory after clearing its content

void *	clearMemory(void * buffer, size_t size, bool freeBuffer)
{
	if (buffer)
	{
		memset(buffer, 0, size);
		if (freeBuffer) free(buffer);
	}
	return NULL;
}
