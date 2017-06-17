$(call PKG_INIT_BIN, v0.3)

$(PKG)_BINARY:=$($(PKG)_DIR)/decoder
$(PKG)_TARGET_BINARY:=$($(PKG)_DEST_DIR)/bin/decoder

$(PKG)_DEPENDS_ON += openssl

ifeq ($(strip $(FREETZ_PACKAGE_DECODER_STATIC)),y)
$(PKG)_LINK_STATIC = 1
override $(PKG)_LIBS = -L$(DECODER_DIR) -lcrypto -lgcc_s
endif

$(PKG)_REBUILD_SUBOPTS += FREETZ_PACKAGE_DECODER_STATIC

$(PKG_LOCALSOURCE_PACKAGE)
$(PKG_CONFIGURED_NOP)

$($(PKG)_BINARY): $($(PKG)_DIR)/.configured
	ln -sf $(TARGET_TOOLCHAIN_STAGING_DIR)/lib/libgcc_pic.a $(DECODER_DIR)/libgcc_s.a
	$(SUBMAKE) -C $(DECODER_DIR) \
		STATIC=$(DECODER_LINK_STATIC) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LIBS="$(DECODER_LIBS)" \
		OPT=""

$($(PKG)_TARGET_BINARY): $($(PKG)_BINARY)
	$(INSTALL_BINARY_STRIP)
	$(SUBMAKE) -C $(DECODER_DIR) \
		INSTALLDIR="$(FREETZ_BASE_DIR)/$(dir $@)" \
		SYMLINK_TARGETS="$(strip $(DECODER_SYMLINKS))" \
		install-links

$(pkg):

$(pkg)-precompiled: $($(PKG)_TARGET_BINARY) 

$(pkg)-clean:
	-$(SUBMAKE) -C $(DECODER_DIR) clean

$(pkg)-uninstall:
	$(SUBMAKE) -C $(DECODER_DIR) \
		INSTALLDIR="$(dir $@)" \
		SYMLINK_TARGETS="$(DECODER_SYMLINKS)" \
		uninstall

$(PKG_FINISH)
