$(call PKG_INIT_BIN, v0.4)

$(PKG)_BINARY:=$($(PKG)_DIR)/decoder
$(PKG)_TARGET_BINARY:=$($(PKG)_DEST_DIR)/bin/decoder

$(PKG)_DEPENDS_ON += $(if $(FREETZ_PACKAGE_DECODER_LIBCRYPTO),openssl,nettle)

$(PKG_LOCALSOURCE_PACKAGE)
$(PKG_CONFIGURED_NOP)

$($(PKG)_BINARY): $($(PKG)_DIR)/.configured
	$(SUBMAKE) -C $(DECODER_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		OPT="" \
		OPENSSL=$(if $(FREETZ_PACKAGE_DECODER_LIBCRYPTO),y,n) \
		STATIC=$(if $(FREETZ_PACKAGE_DECODER_DYNAMIC),n,y)

$($(PKG)_TARGET_BINARY): $($(PKG)_BINARY)
	$(INSTALL_BINARY_STRIP)

$(pkg):

$(pkg)-precompiled: $($(PKG)_TARGET_BINARY) 

$(pkg)-clean:
	-$(SUBMAKE) -C $(DECODER_DIR) clean

$(pkg)-uninstall:
	$(SUBMAKE) -C $(DECODER_DIR) \
		bindir="$(dir $@)" \
		uninstall

$(PKG_FINISH)
