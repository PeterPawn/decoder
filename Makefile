# project
#
BASENAME:=decoder
#
# binary directory
#
SRC_DIR=src
#
# shell scripts directory
#
SCRIPTS_DIR=scripts
#
# targets to make
#
.PHONY: all clean
#
all:
	$(MAKE) -C $(SRC_DIR) libnettle
	$(MAKE) -C $(SRC_DIR)

install:
	$(MAKE) -C $(SRC_DIR) install

install-scripts:
	$(MAKE) -C $(SCRIPTS_DIR) install

clean: 
	$(MAKE) -C $(SRC_DIR) clean
