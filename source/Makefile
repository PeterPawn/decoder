# project
#
BASENAME:=decoder
#
# target binary, used as proxy too
# 
BINARY:=$(BASENAME)
#
# source files
#
BIN_SRCS = decoder.c
HDR = $(BASENAME).h
BIN_OBJS = $(BIN_SRCS:%.c=%.o)
#
# tools
#
CC = gcc
RM = rm
#
# flags for calling the tools
#
#override CFLAGS += -W -Wall -std=c99 -O2 -fvisibility=hidden 
override CFLAGS += -W -Wall -std=c99 -O0 -ggdb -fvisibility=hidden
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
all: $(BINARY)
#
# the CLI binary
#
$(BINARY): $(BIN_OBJS)
	$(CC) $(LDFLAGS) $(filter %.o,$<) -L. -o $@ $(LIBS)
#
# everything to make, if header file changes
#
$(BIN_OBJS): $(HDR)
#
# cleanup 	
#
clean:
	-$(RM) *.o $(BINARY) 2>/dev/null
