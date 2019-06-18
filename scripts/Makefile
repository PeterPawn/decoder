# SPDX-License-Identifier: GPL-2.0-or-later
#
# targets to make
#
.PHONY:	install $(call LINK_NAMES,$(LINKS))
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
LINKS += (decode_passwords, decode_secrets)
LINKS += (test123 test456, test2)
#
# ANSI control sequences for colored messages
#
MSGGREEN:=$(shell printf "\033[32m")
MSGRED:=$(shell printf "\033[31m")
MSGRESET:=$(shell printf "\033[0m")
MSGBOLD:=$(shell printf "\033[1m")
define MSG_NEWLINE
$(info $(shell printf "\n\n"))
endef
#
# check TARGET variable
#
ifeq ($(TARGET),)
$(error $(MSGRED)$(MSGBOLD)Missing TARGET definition.$(MSGRESET))
endif
#
# use shell to extract from LINKS definition
#
OPAR:=(
CPAR:=)
define LINK_NAMES
$(shell printf "%s" "$(1)" | sed -n -e "s|$(OPAR)\$(OPAR)[^,]*\$(CPAR),[^$(CPAR)]*$(CPAR)|\1|gp")
endef
define LINK_ENTRIES
$(shell printf "%s" "$(1)" | sed -n -e "s|$(OPAR)|\"$(OPAR)|g;s|$(CPAR)|$(CPAR)\"|gp")
endef
#
# show install destination
#
$(info $(MSGGREEN)$(MSGBOLD)install directory:$(MSGRESET) $(MSGBOLD)$(TARGET)$(MSGRESET))
$(call MSG_NEWLINE)
$(info ======================================)
#
# install to TARGET directory (which has to exist already)
#
# do not use 'install' utility here, because it's missing on FRITZ!OS devices as well (where an existing
# 'make' would be a miracle already)
#
install-scripts: $(SCRIPTS)
	@test -d $(TARGET) || (printf "Target directory '$(TARGET)' does not exist.\n" 1>&2 && false)
	cp -av $^ $(TARGET)

install-links:
	@for links in $(call LINK_ENTRIES,$(LINKS)); do \
		target="$$(printf "%s" "$$links" | sed -n -e "s|.*,[ ]*\(.[^)]*\))|\1|gp")"; \
		link="$$(printf "%s" "$$links" | sed -n -e "s|(\([^,]*\),.*|\1|gp")"; \
		for l in $$link; do \
			printf "Linking '$(TARGET)/$$l' to '$(TARGET)/$$target' ...\n" 1>&2; \
			opwd=$$PWD; \
			cd $(TARGET); \
			ln -s $$target $$l; \
			cd $$opwd; \
		done; \
	done

install: install-scripts install-links
#
# uninstall
#
uninstall: 
	@for name in $(call LINK_NAMES,$(LINKS)) $(SCRIPTS); do \
		printf "Removing $(TARGET)/$$name\n" 1>&2; \
		rm $(TARGET)/$$name 2>/dev/null; \
	done; \
	true
