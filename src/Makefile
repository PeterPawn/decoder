###########################################################
#                                                         #
# project settings                                        #
#                                                         #
###########################################################
BASENAME:=decoder
#
# target binary
# 
BINARY:=$(BASENAME)
#
# source files
#
SRCS = decoder.c
HDRS = decoder.h
#
# link binaries with this libraries too
#
LIBS = -lcrypto
#
# target directory, if not overridden
#
INSTALLDIR = ../bin
#
# symlinks names for binary utility
#
SYMLINK_TARGETS += b32dec
SYMLINK_TARGETS += b32enc
SYMLINK_TARGETS += b64dec
SYMLINK_TARGETS += b64enc
SYMLINK_TARGETS += hexdec
SYMLINK_TARGETS += hexenc
SYMLINK_TARGETS += user_password
SYMLINK_TARGETS += device_password
SYMLINK_TARGETS += password_from_device
SYMLINK_TARGETS += decode_export
SYMLINK_TARGETS += decode_secrets
SYMLINK_TARGETS += decode_secret
SYMLINK_TARGETS += decode_passwords
###########################################################
#                                                         #
# tool definitions, will be overridden for cross builds   #
#                                                         #
###########################################################
CC = gcc
RM = rm
STRIP = strip
INSTALL = install
#
# flags for calling the tools
#
override CFLAGS += -W -Wall -std=c99 -fvisibility=hidden
ifdef DEBUG
override CFLAGS += -O0 -ggdb -DDEBUG
else
override CFLAGS += -O2 -ffunction-sections -fdata-sections
override LDFLAGS += -Wl,--gc-sections
STRIP_BINARY = strip
endif
ifeq ($(STATIC), 1)
override CFLAGS += -DSTATIC -static
override LDFLAGS += -Wl,-static
endif
#
# how to build objects from sources
#
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
###########################################################
#                                                         #
# prepare variables                                       #
#                                                         #
###########################################################
DESTDIR:=$(shell printf "%s" "$(INSTALLDIR)" | sed -e 's|^\(.*\)/\+$$|\1|')/
###########################################################
#                                                         #
# possible targets to make                                #
#                                                         #
###########################################################
.PHONY: all
#
all: $(BINARY)
#
$(SRCS:%.c=%.o): $(HDRS) Makefile
#
$(BINARY): $(SRCS:%.c=%.o)
	$(CC) $(LDFLAGS) $(EXTRA_LDFLAGS) $(filter %.o,$<) -L. -o $@ $(LIBS)
#
strip:	$(BINARY)
	$(STRIP) $(BINARY)
#
install: install_$(BINARY) $(SYMLINK_TARGETS)
#
install_$(BINARY): $(BINARY) $(STRIP_BINARY)
	$(INSTALL) -d $(DESTDIR)
	$(INSTALL) $(BINARY) $(DESTDIR)
#
install-links: $(SYMLINK_TARGETS)
#
$(SYMLINK_TARGETS): $(BINARY)
	ln -sf $(BINARY) $(DESTDIR)$@
#
uninstall:
	-@for l in $(SYMLINK_TARGETS); do [ -L $(DESTDIR)$$l ] && $(RM) $(DESTDIR)$$l && echo "symlink for $$l removed"; done; true
	-[ -f $(DESTDIR)$(BINARY) ] && $(RM) $(DESTDIR)$(BINARY) 2>/dev/null || true
#
clean:
	-[ -f $(BINARY) ] && $(RM) $(BINARY) 2>/dev/null || true
	-[ -f *.o ] && $(RM) *.o 2>/dev/null || true
