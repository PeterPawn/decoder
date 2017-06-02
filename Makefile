# project
#
BASENAME:=decode_passwords
#
# binary directory
#
SRC_DIR=src
#
# targets to make
#
.PHONY: all clean
#
all:
	$(MAKE) -C $(SRC_DIR)
#
# cleanup
#
clean:
	$(MAKE) -C $(SRC_DIR) clean
	-$(RM) *.yf_scriptlib 2>/dev/null
