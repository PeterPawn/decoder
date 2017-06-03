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
SCRIPTLIB_SCRIPTS=crypto
SCRIPTLIB_FILES=$(addsuffix .yf_scriptlib, $(SCRIPTLIB_SCRIPTS))
#
# targets to make
#
.PHONY: all clean
#
all:
	$(MAKE) -C $(SRC_DIR)
install:
	$(MAKE) -C $(SRC_DIR) install
clean: 
	$(MAKE) -C $(SRC_DIR) clean
#
# script library
#
prepare-scriptlib: $(SCRIPTLIB_FILES)
clean-scriptlib:
	-$(RM) $(SCRIPTLIB_FILES) 2>/dev/null
$(SCRIPTLIB_FILES): $(SCRIPTLIB_SCRIPTS)
	-$(SHELL) $(basename $@) </dev/null 2>/dev/null 1>&2 || true
