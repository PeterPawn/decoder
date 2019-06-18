# SPDX-License-Identifier: GPL-2.0-or-later

prj=nettle-3.4.1

all:	$(prj) configure build install

$(prj):
	./download.sh

configure: $(prj)
	cd $(prj); \
	umask 077; \
	sed -e '/gdb3/,+6d' -i configure.ac; \
	./configure --disable-shared --disable-public-key --disable-openssl --disable-documentation --enable-mini-gmp --prefix=$$(pwd) libdir=$$(pwd)/lib CFLAGS="-O2 -ffunction-sections -fdata-sections $(CFLAGS)" 

clean:
	@[ -d $(prj) ] && ( cd $(prj); make clean; cd ..; rm -r $(prj) 2>/dev/null ) || true; \
	[ -f ../libnettle.a ] && rm ../libnettle.a 2>/dev/null; \
	[ -d ../include/nettle ] && rm -r ../include/nettle 2>/dev/null; \
	rm -r .gnupg nettle*.tar.gz* 2>/dev/null; \
	printf "Cleanup done.\n" 1>&2
	
build:	$(prj) $(prj)/config.status
	$(MAKE) -C $(prj)

install: $(prj)
	$(MAKE) -C $(prj) install
	cp -a $(prj)/lib/libnettle.a ..
	cp -a $(prj)/include ..
