/*
 * vim: set tabstop=4 syntax=c :
 *
 * multi-call utility to provide the same functionality as the shell scripts
 * from the 'decode_passwords' project, but as a lightning-fast binary
 * implementation
 *
 * Copyright (C) 2014-2017, Peter Haemmerlein (peterpawn@yourfritz.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * PLEASE KEEP IN MIND, THAT THE SAME LICENSE EXCEPTIONS ARE USED FOR THIS 
 * CODE, THAT WERE ESTABLISHED FOR THE SHELL SCRIPTS IN THIS PROJECT. HAVE 
 * A LOOK INTO THE README FILE AT THE PROJECT ROOT DIRECTORY.
 *
 * Otherwise this program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, please look for the file LICENSE.
 */

#define _GNU_SOURCE

#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <dlfcn.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <openssl/evp.h>
#include "decoder.h"

#define EXPORTED                        __attribute__((__visibility__("default")))
#define UNUSED                          __attribute__((unused))

// global verbosity setting

static decoder_verbosity_t 				__decoder_verbosity = VERBOSITY_NORMAL;

#define verbosity_options_long			{ "verbose", no_argument, NULL, 'v' },\
										{ "quiet", no_argument, NULL, 'q' },\
										{ "help", no_argument, NULL, 'h' }
#define verbosity_options_short			"qvh"
#define check_verbosity_options_short()	case 'v':\
											__decoder_verbosity = VERBOSITY_VERBOSE;\
											break;\
										case 'q':\
											__decoder_verbosity = VERBOSITY_SILENT;\
											break
#define invalid_option(opt)				default:\
											errorMessage("Invalid option '%c' specified.\a\n", (char) opt);\
											exit(EXIT_FAILURE)
#define help_option(function)			case 'h':\
											usageScreen_##function ();\
											exit(EXIT_FAILURE)
#define getopt_message_displayed(cmd)	case '?':\
											usageScreen_##cmd ();\
											exit(EXIT_FAILURE);

// global error state

static decoder_error_t					__decoder_error = DECODER_ERROR_NOERROR;

#define setError(err)                   __decoder_error = DECODER_ERROR_##err
#define resetError()					setError(NOERROR)
#define returnError(err,value)          { setError(err); return (value); }
#define getError()						(__decoder_error)
#define getErrorText(err)				(__decoder_error_text[err])
#define isAnyError()					(getError() != DECODER_ERROR_NOERROR)
#define isError(err)					(getError() == DECODER_ERROR_##err)

#define errorMessage(...)				if (__decoder_verbosity != VERBOSITY_SILENT) fprintf(stderr, ##__VA_ARGS__)
#define verboseMessage(...)				if (__decoder_verbosity == VERBOSITY_VERBOSE) fprintf(stderr, ##__VA_ARGS__)

// callable functions table

static commandEntry_t	commands[] = {
	{ .name = "b32dec", .ep = &b32dec_main },
	{ .name = "b32enc", .ep = &b32enc_main },
	{ .name = "b64dec", .ep = &b64dec_main },
	{ .name = "b64enc", .ep = &b64enc_main },
	{ .name = "hexdec", .ep = &hexdec_main },
	{ .name = "hexenc", .ep = &hexenc_main },
	{ .name = "user_password", .ep = &user_password_main },
	{ .name = "device_password", .ep = &device_password_main },
	{ .name = "decode_secret", .ep = &decode_secret_main },
	{ .name = NULL, .ep = NULL }
};

// display usage help

void	usageScreen(void)
{
	errorMessage("decoder for AVM's cipher implementation\n");
}

void	usageScreen_b32dec(void)
{
	errorMessage("help for b32dec\n");
}

void	usageScreen_b32enc(void)
{
	errorMessage("help for b32enc\n");
}

void	usageScreen_b64dec(void)
{
	errorMessage("help for b64dec\n");
}

void	usageScreen_b64enc(void)
{
	errorMessage("help for b64enc\n");
}

void	usageScreen_hexdec(void)
{
	errorMessage("help for hexdec\n");
}

void	usageScreen_hexenc(void)
{
	errorMessage("help for hexenc\n");
}

void	usageScreen_user_password(void)
{
	errorMessage("help for user_password\n");
}

void	usageScreen_device_password(void)
{
	errorMessage("help for device_password\n");
}

void	usageScreen_decode_secret(void)
{
	errorMessage("help for decode_secret\n");
}

// Base32 encoding table

static char * UNUSED	base32Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ123456";

// Base64 encoding table

static char * UNUSED	base64Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// hexadecimal translation table

static char * UNUSED	hexTable = "0123456789ABCDEF";

// subfunctions

size_t	base32ToBinary(char *base32, size_t base32Size, char *binary, size_t binarySize)
{
	uint32_t        offset = 0;
	uint32_t		outOffset = 0;
	size_t			b32Size = (base32Size == (size_t) -1 ? strlen(base32) : base32Size);
	
	if (b32Size % 8)
		returnError(INV_B32_SIZE, 0);
	if ((b32Size * 5 / 8) > binarySize)
		returnError(BUF_TOO_SMALL, (b32Size * 5 / 8));
	while (offset < b32Size && outOffset < binarySize)
	{
		uint32_t		bits = 0;
		uint32_t		value = 0;

		for (uint32_t i = 0; i < 8; i++)
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

size_t	binaryToBase32(char *binary, size_t binarySize, char *base32, size_t base32Size)
{
	uint32_t		offset = 0;
	uint32_t		outOffset = 0;

	if (binarySize % 5)
		returnError(INV_B32_ENC_SIZE, 0);
	if ((binarySize * 8 / 5) > base32Size)
		returnError(BUF_TOO_SMALL, 0);
	while (offset < binarySize && outOffset < base32Size)
	{
		uint32_t	bits = 0;
		uint32_t	value = 0;

		for (uint32_t i = 0; i < 5; i++)
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

size_t	base64ToBinary(char *base64, size_t base64Size, char *binary, size_t binarySize, bool pad)
{
	uint32_t        offset = 0;
	uint32_t		outOffset = 0;
	size_t			b64Size = (base64Size == (size_t) -1 ? strlen(base64) : base64Size);
	bool			filler = false;
	size_t			bSize = (b64Size * 3 / 4);
	uint32_t		bits = 0;
	uint32_t		value = 0;
	
	if (b64Size % 4)
	{
		if (pad)	
			bSize = ((b64Size / 4) + 1) * 3;
		else
			returnError(INV_B64_SIZE, 0);
	}
	if (bSize > binarySize)
		returnError(BUF_TOO_SMALL, bSize);
	while (offset < b64Size && outOffset < binarySize)
	{
		value = 0;
		for (uint32_t i = 0; i < 4 && offset < b64Size; i++)
		{
			char 	c = *(base64 + offset++);
	
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
		if (filler && offset < b64Size)
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

size_t	binaryToBase64(char *binary, size_t binarySize, char *base64, size_t base64Size, bool pad)
{
	uint32_t		offset = 0;
	uint32_t		outOffset = 0;
	uint32_t		bSize = (binarySize * 4 / 3);
	uint32_t		bits = 0;
	uint32_t		value = 0;

	if (binarySize % 3)
		bSize = ((binarySize / 3 ) + 1) * 4;
	if (bSize > base64Size)
		returnError(BUF_TOO_SMALL, 0);
	while (offset < binarySize && outOffset < base64Size)
	{
		value = 0;
		for (uint32_t i = 0; i < 3 && offset < binarySize; i++)
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

size_t 	hexadecimalToBinary(char *input, size_t inputSize, char *output, size_t outputSize)
{
	uint32_t        offset = 0;
	uint32_t		outOffset = 0;
	size_t			inSize = (inputSize == (size_t) -1 ? strlen(input) : inputSize);
	uint32_t		value = 0;
	bool			high = true;
	
	if (inSize & 1)
		returnError(INV_HEX_SIZE, 0);
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
	return outOffset;
}

size_t	binaryToHexadecimal(char *input, size_t inputSize, char *output, size_t outputSize)
{
	uint32_t			index = 0;

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

char *	wrapOutput(bool wrapLines, uint32_t lineSize, uint32_t *charsOnLine, uint32_t *toWrite, char *output)
{
	uint32_t			remOnLine = lineSize - *charsOnLine;
	char *				out = output;

	if (wrapLines && (*toWrite > remOnLine)) /* wrap on lineSize */
	{
		if ((remOnLine > 0) && (fwrite(out, remOnLine, 1, stdout) != 1)) /* remaining line */
			returnError(WRITE_FAILED, out);
		out += remOnLine;
		*toWrite -= remOnLine;
		*charsOnLine = 0;
		if (fwrite("\n", 1, 1, stdout) != 1) /* append newline */
			returnError(WRITE_FAILED, 0);
		while (*toWrite > lineSize)
		{
			if (fwrite(out, lineSize, 1, stdout) != 1)
				returnError(WRITE_FAILED, 0);
			*toWrite -= lineSize;
			out += lineSize;
			if (fwrite("\n", 1, 1, stdout) != 1) /* append newline */
				returnError(WRITE_FAILED, 0);
		}
	}
	return out;
}

// callable functions of the multi-call binary

// 'b32dec' function - decode Base32 encoded data from STDIN to STDOUT

void b32dec_output(char * base32, bool hexOutput)
{
	char				binary[5];
	size_t				binarySize = base32ToBinary(base32, (size_t) -1, binary, sizeof(binary));
	char				hex[10];
	char *				out;
	size_t				outSize;

	if (isAnyError()) /* usually invalid characters */
	{
		if (isError(INV_B32_DATA))
		{
			errorMessage("Invalid data value encountered on STDIN.\a\n");
		}
		else if (isError(INV_B32_SIZE))
		{
			errorMessage("Invalid data size encountered on STDIN.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}
	if (hexOutput)
	{
		outSize = binaryToHexadecimal(binary, binarySize, hex, sizeof(hex));
		out = hex;
	}
	else
	{
		outSize = binarySize;
		out = binary;
	}
	if (fwrite(out, outSize, 1, stdout) != 1)
	{
		setError(WRITE_FAILED);
		errorMessage("Write to STDOUT failed.\a\n");
		exit(EXIT_FAILURE);
	}
}

int b32dec_main(int argc, char** argv, int argo)
{
	char				buffer[81];
	char *				input;
	char				base32[9];
	int					convUsed = 0;
	bool				hexOutput = false;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-output", no_argument, NULL, 'x' },
		};
		char *			options_short = "x" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexOutput = true;
					break;

				check_verbosity_options_short();
				help_option(b32dec);
				getopt_message_displayed(b32dec);
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((input = fgets(buffer, sizeof(buffer), stdin)) != NULL)
	{
		input--;
		while (*(++input))
		{
			if (isspace(*input))
				continue;
			base32[convUsed++] = *input;
			if (convUsed == 8)
			{
				base32[convUsed] = 0;
				b32dec_output(base32, hexOutput);
				convUsed = 0;
			}
		}
	}	

	if (convUsed > 0) /* remaining data exist */
	{
		base32[convUsed] = 0;
		b32dec_output(base32, hexOutput);
	}
	
	return EXIT_SUCCESS;
}

// 'b32enc' function - encode binary data from STDIN to Base32 encoded on STDOUT

int b32enc_main(int argc, char** argv, int argo)
{
	bool				hexInput = false;
	bool				padInput = false;
	char				buffer[20];
	size_t				read = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-input", no_argument, NULL, 'x' },
			{ "pad-input", no_argument, NULL, 'p' },
		};
		char *			options_short = "xp" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexInput = true;
					break;

				case 'p':
					padInput = true;
					break;

				check_verbosity_options_short();
				help_option(b32enc);
				getopt_message_displayed(b32enc);
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
	{
		if (hexInput)
		{
			char	withoutSpaces[sizeof(buffer)];
			size_t	used = 0;
			char *	in;
			char *	out;
			int		i;
			size_t	more = read;

			in = buffer;
			out = withoutSpaces;
			while (more > 0)
			{
				for (i = more; i > 0; i--, in++)
				{
					if (isspace(*(in)))
						continue;
					*(out++) = *in;
					used++;
				}
				if (used == sizeof(withoutSpaces))
					break;
				more = fread(buffer, 1, sizeof(withoutSpaces) - used, stdin);
				if (more == 0)
					break;
				in = buffer;
			}
			read = hexadecimalToBinary(withoutSpaces, used, buffer, sizeof(buffer));
			if (read == 0) 
				break;
		}
		if ((read % 5))
		{
			if (padInput)
			{
				int		r = 5 - (read % 5);
				char *	pad = &buffer[read];

				for (int i = 0; i < r; i++)
				{
					*pad++ = 0;
					read++;
				}
			}
			else
			{
				setError(INV_B32_ENC_SIZE);
				read = 0;
				break;	
			}
		}
		
		char	base32[(sizeof(buffer) / 5 * 8) + 1]; /* one more byte for optional end of string */
		size_t	base32Size = binaryToBase32(buffer, read, base32, sizeof(base32) - 1);

		if (base32Size > 0)
		{
			if (fwrite(base32, base32Size, 1, stdout) != 1)
			{
				setError(WRITE_FAILED);
				errorMessage("Write to STDOUT failed.\a\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	
	if (isAnyError()) 
	{
		if (isError(INV_HEX_DATA))
		{
			errorMessage("Invalid hexadecimal data value encountered on STDIN.\a\n");
		}
		else if (isError(INV_HEX_SIZE))
		{
			errorMessage("Invalid hexadecimal data size encountered on STDIN.\a\n");
		}
		else if (isError(INV_B32_ENC_SIZE))
		{
			errorMessage("Invalid data size encountered on STDIN.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

// 'b64dec' function - decode Base64 encoded data from STDIN to STDOUT

void b64dec_output(char * base64, bool hexOutput, bool pad)
{
	char				binary[3];
	size_t				binarySize = base64ToBinary(base64, (size_t) -1, binary, sizeof(binary), pad);
	char				hex[6];
	char *				out;
	size_t				outSize;

	if (isAnyError()) /* usually invalid characters */
	{
		if (isError(INV_B64_DATA))
		{
			errorMessage("Invalid data value encountered on STDIN.\a\n");
		}
		else if (isError(INV_B64_SIZE))
		{
			errorMessage("Invalid data size encountered on STDIN.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}
	if (hexOutput)
	{
		outSize = binaryToHexadecimal(binary, binarySize, hex, sizeof(hex));
		out = hex;
	}
	else
	{
		outSize = binarySize;
		out = binary;
	}
	if (fwrite(out, outSize, 1, stdout) != 1)
	{
		setError(WRITE_FAILED);
		errorMessage("Write to STDOUT failed.\a\n");
		exit(EXIT_FAILURE);
	}
}

int b64dec_main(int argc, char** argv, int argo)
{
	char				buffer[80];
	char *				input;
	char				base64[5];
	int					convUsed = 0;
	bool				hexOutput = false;
	bool				padOutput = false;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-output", no_argument, NULL, 'x' },
			{ "pad-output", no_argument, NULL, 'p' },
		};
		char *			options_short = "xp" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexOutput = true;
					break;

				case 'p':
					padOutput = true;
					break;

				check_verbosity_options_short();
				help_option(b64dec);
				getopt_message_displayed(b64dec);
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((input = fgets(buffer, sizeof(buffer), stdin)) != NULL)
	{
		input--;
		while (*(++input))
		{
			if (isspace(*input))
				continue;
			base64[convUsed++] = *input;
			if (convUsed == 4)
			{
				base64[convUsed] = 0;
				b64dec_output(base64, hexOutput, padOutput);
				convUsed = 0;
			}
		}
	}	

	if (convUsed > 0) /* remaining data exist */
	{
		base64[convUsed] = 0;
		b64dec_output(base64, hexOutput, padOutput);
	}
	
	return EXIT_SUCCESS;
}

// 'b64enc' function - encode binary data from STDIN to Base64 encoded on STDOUT

int b64enc_main(int argc, char** argv, int argo)
{
	bool				hexInput = false;
	bool				padOutput = false;
	bool				wrapLines = false;
	uint32_t			lineSize = 76;
	uint32_t			charsOnLine = 0;
	char				buffer[120];
	size_t				read = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-input", no_argument, NULL, 'x' },
			{ "pad-output", no_argument, NULL, 'p' },
			{ "wrap-lines", optional_argument, NULL, 'w' },
		};
		char *			options_short = "xpw" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexInput = true;
					break;

				case 'p':
					padOutput = true;
					break;

				case 'w':
					{
						char *	endString = NULL;
						char *	startString;

						wrapLines = true;
						if (optarg && *optarg)
						{
							startString = optarg;	
						}
						else
						{
							if ((optind + 1) >= argc)
								break; /* last option, no number present */
							startString = argv[optind + 1];
							if (*startString == '-')
								break; /* next is an option */
						}
						lineSize = strtoul(startString, &endString, 10);
						if (*startString && strlen(endString))
						{
							errorMessage("Invalid line size '%s' specified for -w option.\a\n", startString);
							return(EXIT_FAILURE);
						}
						else
							optind++;
					}
					break;

				check_verbosity_options_short();
				help_option(b64enc);
				getopt_message_displayed(b64enc);
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
	{
		if (hexInput)
		{
			char	withoutSpaces[sizeof(buffer)];
			size_t	used = 0;
			char *	in;
			char *	out;
			int		i;
			size_t	more = read;

			in = buffer;
			out = withoutSpaces;
			while (more > 0)
			{
				for (i = more; i > 0; i--, in++)
				{
					if (isspace(*(in)))
						continue;
					*(out++) = *in;
					used++;
				}
				if (used == sizeof(withoutSpaces))
					break;
				more = fread(buffer, 1, sizeof(withoutSpaces) - used, stdin);
				if (more == 0)
					break;
				in = buffer;
			}
			read = hexadecimalToBinary(withoutSpaces, used, buffer, sizeof(buffer));
			if (read == 0) 
				break;
		}
		
		char		base64[(sizeof(buffer) * 4 / 3) + 1]; /* one more byte for optional end of string */
		size_t		base64Size = binaryToBase64(buffer, read, base64, sizeof(base64) - 1, padOutput);
		
		if (base64Size == 0) break;
		
		uint32_t	toWrite = base64Size;
		char *		out = base64;

		out = wrapOutput(wrapLines, lineSize, &charsOnLine, &toWrite, out);
		if (isAnyError())
			break;

		if ((toWrite > 0) && fwrite(out, toWrite, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			break;
		}
		if (wrapLines)
		{
			charsOnLine += toWrite;
		}
	}
	if (!isAnyError() && wrapLines && (fwrite("\n", 1, 1, stdout) != 1)) /* append newline */
		setError(WRITE_FAILED);
	
	if (isAnyError()) 
	{
		if (isError(WRITE_FAILED))
		{
			errorMessage("Write to STDOUT failed.\a\n");
		}
		else if (isError(INV_HEX_DATA))
		{
			errorMessage("Invalid hexadecimal data value encountered on STDIN.\a\n");
		}
		else if (isError(INV_HEX_SIZE))
		{
			errorMessage("Invalid hexadecimal data size encountered on STDIN.\a\n");
		}
		else if (isError(INV_B64_ENC_SIZE))
		{
			errorMessage("Invalid data size encountered on STDIN.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

// 'hexdec' function - decode hexadecimal presentation of data from STDIN to STDOUT

int hexdec_main(int argc, char** argv, int argo)
{
	char				buffer[80];
	size_t				read = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
		};
		char *			options_short = verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				check_verbosity_options_short();
				help_option(hexdec);
				getopt_message_displayed(hexdec);
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
	{
		char	withoutSpaces[sizeof(buffer)];
		size_t	used = 0;
		char *	in;
		char *	out;
		int		i;
		size_t	more = read;

		in = buffer;
		out = withoutSpaces;
		while (more > 0)
		{
			for (i = more; i > 0; i--, in++)
			{
				if (isspace(*(in)))
					continue;
				*(out++) = *in;
				used++;
			}
			if (used == sizeof(withoutSpaces))
				break;
			more = fread(buffer, 1, sizeof(withoutSpaces) - used, stdin);
			if (more == 0)
				break;
			in = buffer;
		}
		read = hexadecimalToBinary(withoutSpaces, used, buffer, sizeof(buffer));
		if (read == 0 || isAnyError()) 
			break;

		if (fwrite(buffer, read, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			break;
		}
	}
		
	if (isAnyError()) 
	{
		if (isError(WRITE_FAILED))
		{
			errorMessage("Write to STDOUT failed.\a\n");
		}
		else if (isError(INV_HEX_DATA))
		{
			errorMessage("Invalid hexadecimal data value encountered on STDIN.\a\n");
		}
		else if (isError(INV_HEX_SIZE))
		{
			errorMessage("Invalid hexadecimal data size encountered on STDIN.\a\n");
		}
		else if (isError(INV_B64_ENC_SIZE))
		{
			errorMessage("Invalid data size encountered on STDIN.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

// 'hexenc' function - encode binary data from STDIN to its hexadecimal presentation on STDOUT

int hexenc_main(int argc, char** argv, int argo)
{
	bool				wrapLines = false;
	uint32_t			lineSize = 80;
	uint32_t			charsOnLine = 0;
	char				buffer[120];
	size_t				read = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "wrap-lines", optional_argument, NULL, 'w' },
		};
		char *			options_short = "w" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'w':
					{
						char *	endString = NULL;
						char *	startString;

						wrapLines = true;
						if (optarg && *optarg)
						{
							startString = optarg;	
						}
						else
						{
							if ((optind + 1) >= argc)
								break; /* last option, no number present */
							startString = argv[optind + 1];
							if (*startString == '-')
								break; /* next is an option */
						}
						lineSize = strtoul(startString, &endString, 10);
						if (*startString && strlen(endString))
						{
							errorMessage("Invalid line size '%s' specified for -w option.\a\n", startString);
							return(EXIT_FAILURE);
						}
						else
							optind++;
					}
					break;

				check_verbosity_options_short();
				help_option(hexenc);
				getopt_message_displayed(hexenc);
				invalid_option(opt);
			}
		} 
	}

	resetError();

	while ((read = fread(buffer, 1, sizeof(buffer), stdin)) > 0)
	{
		char		output[(sizeof(buffer) * 2) + 1]; /* one more byte for optional end of string */
		size_t		outputSize = binaryToHexadecimal(buffer, read, output, sizeof(output) - 1);
		
		if (outputSize == 0) break;
		
		uint32_t	toWrite = outputSize;
		char *		out = output;

		out = wrapOutput(wrapLines, lineSize, &charsOnLine, &toWrite, out);
		if (isAnyError())
			break;
		
		if ((toWrite > 0) && fwrite(out, toWrite, 1, stdout) != 1)
		{
			setError(WRITE_FAILED);
			break;
		}
		if (wrapLines)
		{
			charsOnLine += toWrite;
		}
	}
	if (!isAnyError() && wrapLines && (fwrite("\n", 1, 1, stdout) != 1)) /* append newline */
		setError(WRITE_FAILED);
	
	if (isAnyError()) 
	{
		if (isError(WRITE_FAILED))
		{
			errorMessage("Write to STDOUT failed.\a\n");
		}
		else
		{
			errorMessage("Unexpected error %d (%s) encountered.\a\n", getError(), getErrorText(getError()));
		}
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

// 'user_password' function - compute the password hash for export files with a user-specified password

int user_password_main(int argc, char** argv, int argo)
{
	bool				hexOutput = false;
	char *				password = NULL;
	unsigned char		hash[AVM_HASH_SIZE];
	uint32_t			hashLen;
	char				hex[(sizeof(hash) * 2) + 1];
	char *				out;
	size_t				outLen;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-output", no_argument, NULL, 'x' },
		};
		char *			options_short = "x" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexOutput = true;
					break;

				check_verbosity_options_short();
				help_option(user_password);
				getopt_message_displayed(user_password);
				invalid_option(opt);
			}
		}
		if (optind >= argc)
		{
			errorMessage("Missing password on command line.\a\n");
			usageScreen_user_password();
			return EXIT_FAILURE;
		}
		else
			password = argv[optind + 1];
	}
	else
	{
		errorMessage("Missing password on command line.\a\n");
		usageScreen_user_password();
		return EXIT_FAILURE;
	}

	resetError();

	EVP_MD_CTX		*ctx = EVP_MD_CTX_create();
	
	EVP_DigestInit_ex(ctx, EVP_md5(), NULL);
	EVP_DigestUpdate(ctx, password, strlen(password));
	EVP_DigestFinal_ex(ctx, hash, &hashLen);
	EVP_MD_CTX_destroy(ctx);
	EVP_cleanup();

	if (hexOutput)
	{
		outLen = binaryToHexadecimal((char *) hash, hashLen, hex, sizeof(hex));
		out = hex;
	}
	else
	{
		outLen = hashLen;
		out = (char *) hash;
	}
	if (fwrite(out, outLen, 1, stdout) != 1)
	{
		errorMessage("Write to STDOUT failed.\a\n");
	}

	return EXIT_SUCCESS;
}

// 'device_password' function - compute the password hash from the specified device properties

int device_password_main(int argc, char** argv, int argo)
{
	bool				hexOutput = false;
	unsigned char		hash[AVM_HASH_SIZE];
	uint32_t			hashLen;
	char				hex[(sizeof(hash) * 2) + 1];
	char *				out;
	size_t				outLen;
	char *				serial = NULL;
	char *				maca = NULL;
	char *				wlanKey = NULL;
	char *				tr069Passphrase = NULL;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-output", no_argument, NULL, 'x' },
		};
		char *			options_short = "x" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexOutput = true;
					break;

				check_verbosity_options_short();
				help_option(user_password);
				getopt_message_displayed(user_password);
				invalid_option(opt);
			}
		}
		if (optind >= argc)
		{
			errorMessage("Missing password on command line.\a\n");
			return EXIT_FAILURE;
		}
		else
		{
			int		i = optind + 1;
			int		index = 0;

			char *	*arguments[] = {
				&serial,
				&maca,
				&wlanKey,
				&tr069Passphrase,
				NULL
			};

			while (argv[i])
			{
				*(arguments[index++]) = argv[i++];
				if (!arguments[index])
					break;
			}
			if (!maca)
			{
				errorMessage("At least two arguments (serial and maca) are required.\a\n");
				usageScreen_device_password();
				return EXIT_FAILURE;
			}
		}
	}
	else
	{
		errorMessage("Missing arguments on command line.\a\n");
		usageScreen_device_password();
		return EXIT_FAILURE;
	}

	resetError();

	EVP_MD_CTX		*ctx = EVP_MD_CTX_create();
	
	EVP_DigestInit_ex(ctx, EVP_md5(), NULL);
	EVP_DigestUpdate(ctx, serial, strlen(serial));
	EVP_DigestUpdate(ctx, "\n", 1);
	EVP_DigestUpdate(ctx, maca, strlen(maca));
	EVP_DigestUpdate(ctx, "\n", 1);
	if (wlanKey && *wlanKey)
		EVP_DigestUpdate(ctx, wlanKey, strlen(wlanKey));
	if (tr069Passphrase && *tr069Passphrase)
		EVP_DigestUpdate(ctx, tr069Passphrase, strlen(tr069Passphrase));
	EVP_DigestFinal_ex(ctx, hash, &hashLen);
	EVP_MD_CTX_destroy(ctx);
	EVP_cleanup();

	if (hexOutput)
	{
		outLen = binaryToHexadecimal((char *) hash, hashLen, hex, sizeof(hex));
		out = hex;
	}
	else
	{
		outLen = hashLen;
		out = (char *) hash;
	}
	if (fwrite(out, outLen, 1, stdout) != 1)
	{
		errorMessage("Write to STDOUT failed.\a\n");
	}

	return EXIT_SUCCESS;
}

// 'decode_secret' function - decode the specified secret value (in Base32 encoding) perties

int decode_secret_main(int argc, char** argv, int argo)
{
	bool				hexOutput = false;
	char *				out;
	size_t				outLen;
	char *				secret = NULL;
	char *				key = NULL;
	unsigned char 		hash[AVM_HASH_SIZE];
	uint32_t			hashLen = 0;

	if (argc > argo + 1)
	{
		int				opt;
		int				optIndex = 0;

		static struct option options_long[] = {
			verbosity_options_long,
			{ "hex-output", no_argument, NULL, 'x' },
		};
		char *			options_short = "x" verbosity_options_short;

		while ((opt = getopt_long(argc - argo, &argv[argo], options_short, options_long, &optIndex)) != -1)
		{
			switch (opt)
			{
				case 'x':
					hexOutput = true;
					break;

				check_verbosity_options_short();
				help_option(user_password);
				getopt_message_displayed(user_password);
				invalid_option(opt);
			}
		}
		if (optind >= argc)
		{
			errorMessage("Missing password on command line.\a\n");
			return EXIT_FAILURE;
		}
		else
		{
			int		i = optind + 1;
			int		index = 0;

			char *	*arguments[] = {
				&secret,
				&key,
				NULL
			};

			while (argv[i])
			{
				*(arguments[index++]) = argv[i++];
				if (!arguments[index])
					break;
			}
			if (!key)
			{
				errorMessage("Exactly two arguments (base32 encrypted value and hexadecimal key) are required.\a\n");
				usageScreen_decode_secret();
				return EXIT_FAILURE;
			}
		}
	}
	else
	{
		errorMessage("Exactly two arguments (base32 encrypted value and hexadecimal key) are required.\a\n");
		usageScreen_decode_secret();
		return EXIT_FAILURE;
	}

	resetError();

	size_t			secretBufSize = base32ToBinary(secret, (size_t) -1, NULL, 0);
	size_t			keyBufSize = hexadecimalToBinary(key, (size_t) -1, NULL, 0);
	size_t			secretSize = 0;
	size_t			decryptedSize = 0;
	size_t			keySize = 0;
	size_t			dataLen = 0;

	unsigned char *	secretBuffer = (unsigned char *) malloc(secretBufSize);
	unsigned char *	decryptedBuffer = (unsigned char *) malloc(secretBufSize + AVM_BLOCK_SIZE);
	unsigned char *	keyBuffer = (unsigned char *) malloc(AVM_KEY_SIZE);
	char *			hexBuffer = NULL;

	if (!secretBuffer || !decryptedBuffer || !keyBuffer)
	{
		errorMessage("Memory allocation error.\a\n");
		return EXIT_FAILURE;
	}

	memset(secretBuffer, 0, secretBufSize);
	memset(decryptedBuffer, 0, secretBufSize + AVM_BLOCK_SIZE);
	memset(keyBuffer, 0, AVM_KEY_SIZE);
	
	resetError();
	secretSize = base32ToBinary(secret, (size_t) -1, (char *) secretBuffer, secretBufSize);
	keySize = hexadecimalToBinary(key, (size_t) -1, (char *) keyBuffer, keyBufSize);

	if (isAnyError())
	{
		errorMessage("The specified arguments contain invalid data.\a\n");
		return EXIT_FAILURE; /* buffers are freed on exit by the run-time */
	}

	if (keySize != 16)
	{
		errorMessage("The specified key has a wrong size.\a\n");
		return EXIT_FAILURE;
	}

	EVP_CIPHER_CTX		*ctx = EVP_CIPHER_CTX_new();
	EVP_MD_CTX			*hctx = EVP_MD_CTX_create();
	
	EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, keyBuffer, secretBuffer);
	EVP_DecryptUpdate(ctx, decryptedBuffer, (int *) &decryptedSize, secretBuffer + AVM_IV_SIZE, secretSize - AVM_IV_SIZE);

	EVP_DigestInit_ex(hctx, EVP_md5(), NULL);
	EVP_DigestUpdate(hctx, decryptedBuffer + 4, decryptedSize - 4);
	EVP_DigestFinal_ex(hctx, hash, &hashLen);
	
	if (memcmp(decryptedBuffer, hash, 4))
	{	
		setError(INVALID_KEY);
		errorMessage("The specified password is wrong.\a\n");
	}
	else
	{
		dataLen = (*((unsigned char *) decryptedBuffer + 4) << 24) + (*((unsigned char *) decryptedBuffer + 5) << 16) + (*((unsigned char *) decryptedBuffer + 6) << 8) + (*((unsigned char *) decryptedBuffer + 7));
		out = (char *) decryptedBuffer + 8;
	}
	
	EVP_MD_CTX_destroy(hctx);
	EVP_CIPHER_CTX_cleanup(ctx);
	EVP_CIPHER_CTX_free(ctx);
	EVP_cleanup();

	if (!isAnyError())
	{
		if (hexOutput)
		{
			hexBuffer = (char *) malloc((dataLen * 2) * 1);
			if (!hexBuffer)
			{
				errorMessage("Error allocating memory.\a\n");
				setError(NO_MEMORY);
			}
			else
			{
				outLen = binaryToHexadecimal(out, dataLen, hexBuffer, (dataLen * 2) + 1);
				out = hexBuffer;
			}
		}
		else
		{
			outLen = dataLen;
			if (*(out + outLen) == 0) /* C-style string, omit last byte */
				outLen--;
		}
		if (!isAnyError() && fwrite(out, outLen, 1, stdout) != 1)
		{
			errorMessage("Write to STDOUT failed.\a\n");
		}
	}

	if (secretBuffer)
	{
		memset(secretBuffer, 0, secretBufSize);
		free(secretBuffer);
		secretBuffer = NULL;
	}
	if (decryptedBuffer)
	{
		memset(decryptedBuffer, 0, secretBufSize);
		free(decryptedBuffer);
		decryptedBuffer = NULL;
	}
	if (keyBuffer)
	{
		memset(keyBuffer, 0, keyBufSize);
		free(keyBuffer);
		keyBuffer = NULL;
	}
	if (hexBuffer)
	{
		memset(hexBuffer, 0, dataLen * 2);
		free(hexBuffer);
		hexBuffer = NULL;
	}

	return EXIT_SUCCESS;
}

// main entry point for each call

int main(int argc, char** argv)
{
	commandEntry_t *	command = commands;
	int					argumentCount = argc;
	char **				arguments = argv;
	int					argumentOffset = 0;
	char * 				fname;
	char * 				ename;
	char 				enameLong[PATH_MAX];
	
	if (readlink("/proc/self/exe", enameLong, PATH_MAX) == -1)
	{
		errorMessage("Unable to get executable name from procfs.\a\n");
		exit(EXIT_FAILURE);
	}
	if (argumentCount == 0)
	{
		errorMessage("Unable to get invocation name from arguments.\a\n");
		exit(EXIT_FAILURE);
	}
	ename = basename(strdup(enameLong));
	fname = basename(strdup(arguments[0]));
	
	if (strcmp(ename, fname))
	{
		argumentOffset = 0;		
	}
	else if (argumentCount > 1)
	{
		fname = arguments[1];
		argumentOffset = 1;
	}
	else
	{
		usageScreen();
		exit(EXIT_FAILURE);
	}

	while (command->name)
	{
		if (strcmp(fname, command->name))
		{
			command++;
			continue;
		}
		arguments[0] = ename;
		int exitCode = (*command->ep)(argumentCount, arguments, argumentOffset);
		exit(exitCode);
	}

	if (!command->name)
	{
		errorMessage("Unknown function '%s' for '%s' binary.\a\n\n", fname, enameLong);
		usageScreen();
	}

	exit(EXIT_FAILURE);
}
