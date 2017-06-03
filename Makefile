# project
#
BASENAME:=decoder
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
install:
	$(MAKE) -C $(SRC_DIR) install
#
# cleanup
#
clean:
	$(MAKE) -C $(SRC_DIR) clean
	-$(RM) *.yf_scriptlib 2>/dev/null
