# project
#
BASENAME:=decoder
#
# target binary
# 
BINARY:=$(BASENAME)
#
# source files
#
BIN_SRCS = $(BASENAME).c
HDR = $(BASENAME).h
BIN_OBJS = $(BIN_SRCS:%.c=%.o)
#
# symlinks names for binary utility
#
SYMLINKS:=b32dec
SYMLINKS+=b32enc
SYMLINKS+=b64dec
SYMLINKS+=b64enc
SYMLINKS+=hexdec
SYMLINKS+=hexenc
SYMLINKS+=user_password
SYMLINKS+=device_password
SYMLINKS+=password_from_device
SYMLINKS+=decode_export
SYMLINKS+=decode_secrets
SYMLINKS+=decode_secret
#
# tools
#
CC = gcc
RM = rm
#
# flags for calling the tools
#
override CFLAGS += -W -Wall -std=c99 -fvisibility=hidden
ifdef DEBUG
override CFLAGS += -O0 -ggdb -DDEBUG
else
override CFLAGS += -O2
endif
#
# how to build objects from sources
#
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
#
# link binaries with this libraries too
#
LIBS = -lcrypto
#
# targets to make
#
.PHONY: all clean
#
all: $(BINARY) $(SYMLINKS)
#
# the CLI binary
#
$(BINARY): $(BIN_OBJS)
	$(CC) $(LDFLAGS) $(filter %.o,$<) -L. -o $@ $(LIBS)
#
# make symlinks
#
$(SYMLINKS):
	@ln -sf $(BINARY) $@ && echo "symlink for $@ created"
#
# everything to make, if header file changes
#
$(BIN_OBJS): $(HDR)
#
# cleanup 	
#
clean:
	-[ -f $(BINARY) ] && $(RM) $(BINARY) 2>/dev/null || true
	-[ -f *.o ] && $(RM) *.o 2>/dev/null || true
	-@for l in $(SYMLINKS); do [ -L $$l ] && $(RM) $$l && echo "symlink for $$l removed"; done; true
