# SPDX-License-Identifier: GPL-2.0-or-later

prj := nettle-$(NETTLE_VERSION)

.PHONY:	build clean all nettle-headers nettle-lib

all:	$(DESTDIR)/libnettle.a nettle-headers

$(prj):
	./download.sh $(NETTLE_VERSION)

$(prj)/config.status: | $(prj)
	cd $(prj); \
	umask 077; \
	./configure --disable-shared --disable-public-key --disable-openssl --disable-documentation --enable-mini-gmp CFLAGS="-O2 -ffunction-sections -fdata-sections $(CFLAGS)" --prefix=

clean:
	@for d in $$(find . -type d -name "nettle-*"); do [ -d $$d ] && ( cd $$d; make clean; cd ..; rm -r $$d 2>/dev/null ) || true; done; \
	[ -f $(DESTDIR)/libnettle.a ] && rm $(DESTDIR)/libnettle.a 2>/dev/null; \
	rm $(DESTDIR)/libnettle.so* 2>/dev/null && true; \
	[ -d $(DESTDIR)/include/nettle ] && rm -r $(DESTDIR)/include/nettle 2>/dev/null; \
	rm -r .gnupg nettle*.tar.gz* 2>/dev/null; \
	printf "Cleanup done.\n" 1>&2
	
build:	$(prj)/config.status
	$(MAKE) -C $(prj) SUBDIRS=

nettle-shared: build
	$(MAKE) -C $(prj) DESTDIR="$(DESTDIR)" libdir="" install-shared-nettle

nettle-static: build
	$(MAKE) -C $(prj) DESTDIR="$(DESTDIR)" libdir="" install-static

nettle-headers: build
	$(MAKE) -C $(prj) DESTDIR="$(DESTDIR)" install-headers


