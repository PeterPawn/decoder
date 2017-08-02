#
# targets to make
#
.PHONY:	install
#
# script files to copy
#
SCRIPTS += crypto
SCRIPTS += decode_cryptedbinfile
SCRIPTS += decode_export
SCRIPTS += decode_secret
SCRIPTS += decode_secrets
SCRIPTS += device_password
SCRIPTS += password_from_device
SCRIPTS += privatekeypassword
SCRIPTS += user_password
#
# symlinks to create
#
LINKS += decode_passwords=decode_secrets
#
# check TARGET variable
#
ifeq ($(TARGET),)
  $(error Missing TARGET definition.)
endif
#
# show install destination
#
$(info install directory: $(TARGET))
#
# install to TARGET directory (which has to exist already)
#
# do not use 'install' utility here, because it's missing on FRITZ!OS devices as well (where an existing
# 'make' would be a miracle already)
#
install-scripts: $(SCRIPTS)
	@test -d $(TARGET) || (printf "Target directory '$(TARGET)' does not exist.\n" 1>&2 && false)
	cp -av $^ $(TARGET)

install-links: | $(LINKS)

install: install-scripts install-links
#
# uninstall
#
uninstall: | $(patsubst =%,,$(LINKS)) $(SCRIPTS)
	for name in $|; do printf "Removing $(TARGET)/$$name\n"; rm $(TARGET)/$$name 2>/dev/null; done; \
	true
