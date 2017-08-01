# project
#
BASENAME:=decoder
#
# binary directory
#
SRC_DIR=src
#
# shell scripts
#
SCRIPTS=decode_secrets decode_export decode_secret user_password device_password password_from_device crypto
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

clean: 
	$(MAKE) -C $(SRC_DIR) clean
