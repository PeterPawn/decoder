# vim: set tabstop=4 syntax=sh :
###################################################################################################VER#
#                                                                                                     #
# decode_passwords, version 0.3                                                                       #
#                                                                                                     #
# This Makefile is a part of the project from https://github.com/PeterPawn/decode_passwords.          #
#                                                                                                     #
###################################################################################################CPY#
#                                                                                                     #
# Copyright (C) 2014-2017 P.Haemmerlein (peterpawn@yourfritz.de)                                      #
#                                                                                                     #
###################################################################################################LIC#
#                                                                                                     #
# This project is free software, you can redistribute it and/or modify it under the terms of the GNU  #
# General Public License as published by the Free Software Foundation; either version 2 of the        #
# License, or (at your option) any later version.                                                     #
#                                                                                                     #
# This project is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;           #
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.           #
# See the GNU General Public License under http://www.gnu.org/licenses/gpl-2.0.html for more          #
# details.                                                                                            #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# configuration values                                                                                #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# categories and applets                                                                              #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# general converters to/from binary to/from ( Base32 | Base64 | hexadecimal )                         #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_CONVERTERS=y
#######################################################################################################
#                                                                                                     #
# general converters from binary to a textual representation                                          #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_CONVERTERS_ENCODERS=y
#######################################################################################################
#                                                                                                     #
# general converters from text data to binary                                                         #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_CONVERTERS_DECODERS=y
#######################################################################################################
#                                                                                                     #
# general converters for Base32                                                                       #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_CONVERTERS_BASE32=y
#######################################################################################################
#                                                                                                     #
# general converters for Base64                                                                       #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_CONVERTERS_BASE64=y
#######################################################################################################
#                                                                                                     #
# general converters for hexadecimal                                                                  #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_CONVERTERS_HEX=y
#######################################################################################################
#                                                                                                     #
# include Base32 to binary applet                                                                     #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECODE_BASE32=y
#######################################################################################################
#                                                                                                     #
# include Base64 to binary applet                                                                     #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECODE_BASE64=y
#######################################################################################################
#                                                                                                     #
# include hexadecimal to binary applet                                                                #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECODE_HEX=y
#######################################################################################################
#                                                                                                     #
# include binary to Base32 applet                                                                     #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_ENCODE_BASE32=y
#######################################################################################################
#                                                                                                     #
# include binary to Base64 applet                                                                     #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_ENCODE_BASE64=y
#######################################################################################################
#                                                                                                     #
# include binary to hexadecimal applet                                                                #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_ENCODE_HEX=y
#######################################################################################################
#                                                                                                     #
# general decipher key creation                                                                       #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_KEYS=y
#######################################################################################################
#                                                                                                     #
# decryption key from user password applet                                                            #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_KEYS_USER_PASSWORD=y
#######################################################################################################
#                                                                                                     #
# decryption key from device properties applet                                                        #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_KEYS_DEVICE_PROPERTIES=y
#######################################################################################################
#                                                                                                     #
# decryption key from current device properties applet                                                #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_KEYS_CURRENT_DEVICE=y
#######################################################################################################
#                                                                                                     #
# export file handling                                                                                #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECRYPT_EXPORT_FILES=y
#######################################################################################################
#                                                                                                     #
# split export file                                                                                   #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECOMPOSE_EXPORT_FILES=y
#######################################################################################################
#                                                                                                     #
# export file - CRYPTEDBINFILE sections decryption                                                    #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECRYPT_EXPORT_BINFILE=y
#######################################################################################################
#                                                                                                     #
# single value decryption                                                                             #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECRYPT_SINGLE_VALUES=y
#######################################################################################################
#                                                                                                     #
# decrypt any values in a text file                                                                   #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECRYPT_FILES=y
#######################################################################################################
#                                                                                                     #
# compute secret key to decipher private key of box certificate                                       #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_PRIVKEY_PASSWORD=y
#######################################################################################################
#                                                                                                     #
# compute a CRC-32 value for input data                                                               #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_CRC_FILE=y
#######################################################################################################
#                                                                                                     #
# applet names, multiple names may be specified for the same applet                                   #
# for applet names with a preceding plus sign (+), symbolic links will be create on installation      #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# Base32 to binary applet name                                                                        #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECODE_BASE32_NAME="+b32dec"
#######################################################################################################
#                                                                                                     #
# binary to Base32 applet name                                                                        #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_ENCODE_BASE32_NAME="+b32enc"
#######################################################################################################
#                                                                                                     #
# Base64 to binary applet name                                                                        #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECODE_BASE64_NAME="+b64dec"
#######################################################################################################
#                                                                                                     #
# binary to Base64 applet name                                                                        #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_ENCODE_BASE64_NAME="+b64enc"
#######################################################################################################
#                                                                                                     #
# hexadecimal to binary applet name                                                                   #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECODE_HEX_NAME="+hexdec"
#######################################################################################################
#                                                                                                     #
# binary to hexadecimal applet name                                                                   #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_ENCODE_HEX_NAME="+hexenc"
#######################################################################################################
#                                                                                                     #
# key from user password applet name                                                                  #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_KEYS_USER_PASSWORD_NAME="+user_password"
#######################################################################################################
#                                                                                                     #
# key from device properties applet name                                                              #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_KEYS_DEVICE_PROPERTIES_NAME="+device_password"
#######################################################################################################
#                                                                                                     #
# key from current device properties applet name                                                      #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_KEYS_CURRENT_DEVICE_NAME="+password_from_device"
#######################################################################################################
#                                                                                                     #
# decrypt export file applet name                                                                     #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECRYPT_EXPORT_FILES_NAME="+decode_export"
#######################################################################################################
#                                                                                                     #
# split an export file into the contained settings files                                              #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECOMPOSE_EXPORT_FILES_NAME="+split_export"
#######################################################################################################
#                                                                                                     #
# decrypt CRYPTEDBINFILE content applet name                                                          #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECRYPT_EXPORT_BINFILE_NAME="+decode_cryptedbinfile"
#######################################################################################################
#                                                                                                     #
# single value decryption applet name                                                                 #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECRYPT_SINGLE_VALUES_NAME="+decode_secret"
#######################################################################################################
#                                                                                                     #
# decrypt any values in a text file applet name                                                       #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_DECRYPT_FILES_NAME="+decode_secrets +decode_passwords"
ifeq ($(strip $(FREETZ_PACKAGE_DECRYPT_FRITZOS_CFG)), y)
DECODER_CONFIG_DECRYPT_FILES_NAME+="+decrypt-fritzos-cfg"
endif
#######################################################################################################
#                                                                                                     #
# private key password applet name                                                                    #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_PRIVKEY_PASSWORD_NAME="+privatekeypassword"
#######################################################################################################
#                                                                                                     #
# compute CRC-32 value for input file                                                                 #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_CRC_FILE_NAME="+checksum"
#######################################################################################################
#                                                                                                     #
# default memory buffer size                                                                          #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_MEMORY_BUFFER_SIZE=8192
#######################################################################################################
#                                                                                                     #
# default line size, if output to STDOUT should wrap lines                                            #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_WRAP_LINE_SIZE=80
#######################################################################################################
#                                                                                                     #
# default line size, if output to STDOUT should wrap lines                                            #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_URLADER_ENVIRONMENT_PATH="/proc/sys/urlader/environment"
#######################################################################################################
#                                                                                                     #
# show usage screen on parameter errors                                                               #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_AUTO_USAGE=n
#######################################################################################################
#                                                                                                     #
# warn on wrong length of TR-069 passphrases (not 8 or 12 characters)                                 #
#                                                                                                     #
#######################################################################################################
DECODER_CONFIG_WARN_ON_TR069_PASSPHRASE=y
#######################################################################################################
#                                                                                                     #
# create a statically linked binary, senseless with libcrypto - the binary would be very, very large  #
#                                                                                                     #
#######################################################################################################
ifeq "$(strip $(STATIC))" "n"
DECODER_CONFIG_LINK_STATIC=n
ifeq "$(strip $(OPENSSL))" "y"
DECODER_CONFIG_LIBNETTLE=n
else
DECODER_CONFIG_LIBNETTLE=y
endif
else
DECODER_CONFIG_LINK_STATIC=y
DECODER_CONFIG_LIBNETTLE=y
endif
ifeq "$(strip $(OPENSSL))" "y"
DECODER_CONFIG_LIBNETTLE=n
DECODER_CONFIG_LINK_STATIC=n
endif
#######################################################################################################
#                                                                                                     #
# use OpenSSL or Nettle library                                                                       #
#                                                                                                     #
#######################################################################################################
ifeq "$(strip $(DECODER_CONFIG_LIBNETTLE))" "y"
DECODER_CONFIG_LIBCRYPTO=n
else
DECODER_CONFIG_LIBCRYPTO=y
endif
#######################################################################################################
#                                                                                                     #
# include debug info on build                                                                         #
#                                                                                                     #
#######################################################################################################
ifdef DEBUG
DECODER_CONFIG_DEBUG=y
endif
#######################################################################################################
#                                                                                                     #
# Makefile variables composition                                                                      #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# project settings                                                                                    #
#                                                                                                     #
#######################################################################################################
project := decoder
srcdir = .
#######################################################################################################
#                                                                                                     #
# build settings from configuration above                                                             #
#                                                                                                     #
#######################################################################################################
FILES :=
LINKS :=
CMDS :=
CFG :=
APPLET_SRCS := b32dec b32enc b64dec b64enc hexdec hexenc userpw devpw pwfrdev decsngl decfile decexp deccb pkpwd checksum decompose
#######################################################################################################
#                                                                                                     #
# macros to add an applet                                                                             #
#                                                                                                     #
#######################################################################################################
define GEN_FUNCTION
$(shell printf "\"%s\",\n" "$(2)" >>$(addsuffix _commands.c,$(1)))
endef

define ADD_APPLET_MACRO
ifeq "$(strip $(1))" "y"
CMDS += $(if $(filter +%,$(2)),$(3))
LINKS += $(patsubst +%, %, $(filter +%, $(2)))
FILES += $(3)
$(shell $(RM) $(addsuffix _commands.c,$(3)) 2>/dev/null)
$(foreach cmd,$(2:+%=%),$(call GEN_FUNCTION,$(3),$(cmd)))
endif
endef

define ADD_APPLET
$(eval $(call ADD_APPLET_MACRO,$($(addprefix DECODER_CONFIG_, $(1))),$(subst $(shell printf '\042'),,$($(addsuffix _NAME, $(addprefix DECODER_CONFIG_, $(1))))),$(2)))
endef

define IS_SET_MACRO
$(if $(filter y, $(split, $(addprefix DECODER_CONFIG_, $(1)))))
endef

define GEN_CONFIG_LINE
printf "#define\\t%s\\t%s\\n" '$(1)' '$(subst +,,$($(1)))';
endef

define GEN_CONFIG
$(foreach v, $1, $(call GEN_CONFIG_LINE,$(v)))
endef

#######################################################################################################
#                                                                                                     #
# configure conditional inclusion of applets                                                          #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# decrypt data applets                                                                                #
#                                                                                                     #
#######################################################################################################
ifeq ($(DECODER_CONFIG_DECRYPT_FILES),y)
$(call ADD_APPLET,DECRYPT_FILES,decfile)
endif
ifeq ($(DECODER_CONFIG_DECRYPT_EXPORT_FILES),y)
$(call ADD_APPLET,DECRYPT_EXPORT_FILES,decexp)
ifeq ($(DECODER_CONFIG_DECRYPT_EXPORT_BINFILE),y)
$(call ADD_APPLET,DECRYPT_EXPORT_BINFILE,deccb)
endif
ifeq ($(DECODER_CONFIG_DECOMPOSE_EXPORT_FILES),y)
$(call ADD_APPLET,DECOMPOSE_EXPORT_FILES,decompose)
endif
endif
ifeq ($(DECODER_CONFIG_DECRYPT_SINGLE_VALUES),y)
$(call ADD_APPLET,DECRYPT_SINGLE_VALUES,decsngl)
endif
#######################################################################################################
#                                                                                                     #
# private key password applet                                                                         #
#                                                                                                     #
#######################################################################################################
ifeq ($(DECODER_CONFIG_PRIVKEY_PASSWORD),y)
$(call ADD_APPLET,PRIVKEY_PASSWORD,pkpwd)
endif
#######################################################################################################
#                                                                                                     #
# CRC-32 applet                                                                                       #
#                                                                                                     #
#######################################################################################################
ifeq ($(DECODER_CONFIG_CRC_FILE),y)
$(call ADD_APPLET,CRC_FILE,checksum)
endif
#######################################################################################################
#                                                                                                     #
# decrypt key applets                                                                                 #
#                                                                                                     #
#######################################################################################################
ifeq ($(DECODER_CONFIG_KEYS),y)
ifeq ($(DECODER_CONFIG_KEYS_USER_PASSWORD),y)
$(call ADD_APPLET,KEYS_USER_PASSWORD,userpw)
endif
ifeq ($(DECODER_CONFIG_KEYS_DEVICE_PROPERTIES),y)
$(call ADD_APPLET,KEYS_DEVICE_PROPERTIES,devpw)
endif
ifeq ($(DECODER_CONFIG_KEYS_CURRENT_DEVICE),y)
$(call ADD_APPLET,KEYS_CURRENT_DEVICE,pwfrdev)
endif
endif
#######################################################################################################
#                                                                                                     #
# converter applets                                                                                   #
#                                                                                                     #
#######################################################################################################
ifeq "$(strip $(DECODER_CONFIG_CONVERTERS))" "y"
ifeq "$(strip $(DECODER_CONFIG_CONVERTERS_BASE32))" "y"
ifneq "$(strip $(DECODER_CONFIG_CONVERTERS_DECODERS))" "n"
$(call ADD_APPLET,DECODE_BASE32,b32dec)
endif
ifneq ($(DECODER_CONFIG_CONVERTERS_ENCODERS),n)
ifeq "$(strip $(DECODER_CONFIG_ENCODE_BASE32))" "y"
$(call ADD_APPLET,ENCODE_BASE32,b32enc)
endif
endif
endif
ifeq ($(DECODER_CONFIG_CONVERTERS_BASE64),y)
ifneq ($(DECODER_CONFIG_CONVERTERS_DECODERS),n)
ifneq ($(DECODER_CONFIG_DECODE_BASE64),n)
$(call ADD_APPLET,DECODE_BASE64,b64dec)
endif
endif
ifneq ($(DECODER_CONFIG_CONVERTERS_ENCODERS),n)
ifneq ($(DECODER_CONFIG_ENCODE_BASE64),n)
$(call ADD_APPLET,ENCODE_BASE64,b64enc)
endif
endif
endif
ifeq ($(DECODER_CONFIG_CONVERTERS_HEX),y)
ifneq ($(DECODER_CONFIG_CONVERTERS_DECODERS),n)
ifneq ($(DECODER_CONFIG_DECODE_HEX),n)
$(call ADD_APPLET,DECODE_HEX,hexdec)
endif
endif
ifneq ($(DECODER_CONFIG_CONVERTERS_ENCODERS),n)
ifneq ($(DECODER_CONFIG_ENCODE_HEX),n)
$(call ADD_APPLET,ENCODE_HEX,hexenc)
endif
endif
endif
endif
#######################################################################################################
#                                                                                                     #
# common files as dependencies for other files                                                        #
#                                                                                                     #
#######################################################################################################
CONFIG = config.h
APPLETS = applets.h
HDRS_DEP = common.h
#######################################################################################################
#                                                                                                     #
# source files defined so far are applets and have a '%_usage.c' file with help text                  #
#                                                                                                     #
#######################################################################################################
FILES_APP = $(FILES)
HDRS_APP = $(addsuffix .h, $(FILES_APP))
SRCS_USE = $(addsuffix _usage.c, $(FILES_APP))
OBJS_APP = $(addsuffix .o, $(FILES_APP))
SRCS_APP = $(addsuffix .c, $(FILES_APP))
#######################################################################################################
#                                                                                                     #
# configuration independent source files                                                              #
#                                                                                                     #
#######################################################################################################
FILES_COMMON += errors
FILES_COMMON += output
FILES_COMMON += base32
FILES_COMMON += base64
FILES_COMMON += hex
FILES_COMMON += encryption
FILES_COMMON += exportfile
FILES_COMMON += memory
FILES_COMMON += functions
ifeq "$(strip $(DECODER_CONFIG_LIBNETTLE))" "y"
FILES_COMMON += crypto_nettle
else
FILES_COMMON += crypto_ossl
endif
FILES_COMMON += options
FILES_COMMON += environ
FILES_COMMON += license
FILES_COMMON += crc32
FILES_COMMON += help
HDRS_COMMON = $(addsuffix .h, $(FILES_COMMON))
OBJS_COMMON = $(addsuffix .o, $(FILES_COMMON))
SRCS_COMMON = $(addsuffix .c, $(FILES_COMMON))
#######################################################################################################
#                                                                                                     #
# additional definitions for the main file                                                            #
#                                                                                                     #
#######################################################################################################
FILES_MAIN = decoder
HDRS_MAIN = $(addsuffix .h, $(FILES_MAIN))
OBJS_MAIN = $(addsuffix .o, $(FILES_MAIN))
SRCS_MAIN = $(addsuffix .c, $(FILES_MAIN))
USE_MAIN = $(addsuffix _usage.c, $(FILES_MAIN))
#######################################################################################################
#                                                                                                     #
# configuration file settings                                                                         #
#                                                                                                     #
#######################################################################################################
CFG += CONVERTERS
CFG += CONVERTERS_ENCODERS
CFG += CONVERTERS_DECODERS
CFG += CONVERTERS_BASE32
CFG += CONVERTERS_BASE64
CFG += CONVERTERS_HEX
CFG += DECODE_BASE32
CFG += DECODE_BASE64
CFG += DECODE_HEX
CFG += ENCODE_BASE32
CFG += ENCODE_BASE64
CFG += ENCODE_HEX
CFG += KEYS
CFG += KEYS_USER_PASSWORD
CFG += KEYS_DEVICE_PROPERTIES
CFG += KEYS_CURRENT_DEVICE
CFG += DECRYPT_EXPORT_FILES
CFG += DECRYPT_EXPORT_BINFILE
CFG += DECRYPT_SINGLE_VALUES
CFG += DECRYPT_FILES
CFG += DECODE_BASE32_NAME
CFG += DECODE_BASE64_NAME
CFG += DECODE_HEX_NAME
CFG += ENCODE_BASE32_NAME
CFG += ENCODE_BASE64_NAME
CFG += ENCODE_HEX_NAME
CFG += KEYS_USER_PASSWORD_NAME
CFG += KEYS_DEVICE_PROPERTIES_NAME
CFG += KEYS_CURRENT_DEVICE_NAME
CFG += DECRYPT_EXPORT_FILES_NAME
CFG += DECOMPOSE_EXPORT_FILES_NAME
CFG += DECRYPT_EXPORT_BINFILE_NAME
CFG += DECRYPT_SINGLE_VALUES_NAME
CFG += DECRYPT_FILES_NAME
CFG += PRIVKEY_PASSWORD_NAME
CFG += CRC_FILE_NAME
CFG += MEMORY_BUFFER_SIZE
CFG += WRAP_LINE_SIZE
CFG += URLADER_ENVIRONMENT_PATH
ifeq "$(strip $(DECODER_CONFIG_AUTO_USAGE))" "y"
CFG += AUTO_USAGE
endif
ifeq "$(strip $(DECODER_CONFIG_WARN_ON_TR069_PASSPHRASE))" "y"
CFG += WARN_ON_TR069_PASSPHRASE
endif
ifdef DECODER_CONFIG_LINK_STATIC
CFG += LINK_STATIC
endif
ifdef DECODER_CONFIG_DEBUG
CFG += DEBUG
endif
#######################################################################################################
#                                                                                                     #
# additional include directories                                                                      #
#                                                                                                     #
#######################################################################################################
ifeq "$(strip $(DECODER_CONFIG_LIBNETTLE))" "y"
override CFLAGS += -Iinclude/nettle -DNETTLE
endif
#######################################################################################################
#                                                                                                     #
# additional libraries for linker calls                                                               #
#                                                                                                     #
#######################################################################################################
ifeq "$(strip $(DECODER_CONFIG_LIBNETTLE))" "y"
LIBS = -lnettle
else
LIBS = -lcrypto
endif
#######################################################################################################
#                                                                                                     #
# target binary name                                                                                  #
#                                                                                                     #
#######################################################################################################
binfile := $(project)
#######################################################################################################
#                                                                                                     #
# install binary and symlinks here, override this for cross-builds                                    #
#                                                                                                     #
#######################################################################################################
prefix = $(HOME)
bindir = $(prefix)/bin
#######################################################################################################
#                                                                                                     #
# tools definitions, override them for cross-compiles                                                 #
#                                                                                                     #
#######################################################################################################
MF = Makefile
CC = gcc
RM = rm
STRIP = strip
INSTALL = install
LN = ln
#######################################################################################################
#                                                                                                     #
# tools flags, override them for cross-builds                                                         #
#                                                                                                     #
#######################################################################################################
override CFLAGS += -W -Wall -Wformat=2 -std=c99 -fvisibility=hidden
ifeq ($(DECODER_CONFIG_DEBUG),y)
override CFLAGS += -O0 -ggdb -DDEBUG
else
override CFLAGS += -O2 -ffunction-sections -fdata-sections
override LDFLAGS += -Wl,--gc-sections
endif
#######################################################################################################
#                                                                                                     #
# additional settings for static builds                                                               #
#                                                                                                     #
#######################################################################################################
ifeq ($(DECODER_CONFIG_LINK_STATIC),y)
override CFLAGS += -DSTATIC -static
override LDFLAGS += -static
endif
#######################################################################################################
#                                                                                                     #
# show values on debug                                                                                #
#                                                                                                     #
#######################################################################################################
ifdef $(DEBUG)
newline = $(shell printf "\n")
$(info Settings: $(newline))
$(info FILES=$(FILES))
$(info LINKS=$(LINKS))
$(info CMDS=$(CMDS))
$(info DEBUG=$(DECODER_CONFIG_DEBUG))
$(info $(newline))
endif
#######################################################################################################
#                                                                                                     #
# rules and dependencies                                                                              #
#                                                                                                     #
#######################################################################################################
#                                                                                                     #
# build targets and rules                                                                             #
#                                                                                                     #
#######################################################################################################
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all:	$(binfile)

.PHONY:	all install_$(binfile) links uninstall clean install $(CMDS)

$(OBJS_COMMON):	$(APPLETS) $(CONFIG) $(HDRS_COMMON) $(SRCS_COMMON)

$(OBJS_APP):	$(APPLETS) $(CONFIG) $(HDRS_APP) $(SRCS_APP) $(SRCS_USE)

$(OBJS_MAIN):	$(APPLETS) $(CONFIG) $(HDRS_MAIN) $(SRCS_MAIN) $(USE_MAIN)

$(binfile):	$(OBJS_COMMON) $(OBJS_APP) $(OBJS_MAIN)
	$(CC) $(LDFLAGS) -L. -o $@ $(filter %.o,$^) $(LIBS)

$(APPLETS): | $(CMDS)
	@$(RM) $@ 2>/dev/null || true
	@for l in $(CMDS); do \
		printf "&%s_command,\n" "$$l"; \
	done > $@

$(CONFIG):	| $(APPLETS)
	@(sed -n -e '1,\|^// configuration area$$|p' config.in; \
		$(call GEN_CONFIG, $(addprefix DECODER_CONFIG_, $(CFG))) \
		sed -n -e '\|^// configuration area$$|,$$p' config.in) > $@ ;

libnettle:
	$(MAKE) -C nettle all CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"

#######################################################################################################
#                                                                                                     #
# install targets and rules                                                                           #
#                                                                                                     #
#######################################################################################################
#.PHONY:	$(LINKS:%=$(bindir)/%)
#
#$(LINKS:%=$(bindir)/%):	$(binfile)
#	opwd=$$(pwd); \
#	cd $(bindir); \
#	$(LN) -s -f $< $@ ; \
#	cd $$opwd
#
#install_$(binfile):	$(binfile)
#	$(STRIP) $<
#	[ -d $(bindir) ] || $(INSTALL) -d $(bindir)
#	$(INSTALL) $(binfile) $(bindir)
#
#links:	$(LINKS:%=$(bindir)/%)
#
#install:	install_$(binfile) links
#
#######################################################################################################
#                                                                                                     #
# uninstall / clean targets and rules                                                                 #
#                                                                                                     #
#######################################################################################################
uninstall:
	@for l in $(LINKS:%=$(bindir)/%); do \
		[ -L $$l ] && $(RM) $$l && echo "symlink for $$l removed"; \
	done; \
	true;
	@[ -f $(bindir)/$(binfile) ] && $(RM) $(bindir)/$(binfile) 2>/dev/null || true
	@echo "uninstall succeeded"

clean:
	@[ -f $(binfile) ] && $(RM) $(binfile) 2>/dev/null || true
	@$(RM) $(CONFIG) $(APPLETS) $(OBJS_COMMON) $(OBJS_APP) $(OBJS_MAIN) $(CONFIG) 2>/dev/null || true
	@for n in $(APPLET_SRCS); do \
		$(RM) $${n}_commands.c 2>/dev/null; \
	done; \
	$(MAKE) -C nettle clean; \
	true
	@echo "Cleanup done."

#######################################################################################################
#                                                                                                     #
# end of Makefile                                                                                     #
#                                                                                                     #
#######################################################################################################
