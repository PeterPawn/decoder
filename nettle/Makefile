prj=nettle-3.3

all:	$(prj) configure build install

$(prj):
	./download.sh

configure:
	cd $(prj); \
	umask 077; \
	sed -e '/gdb3/,+6d' -i configure.ac; \
	./configure --disable-shared --disable-public-key --disable-openssl --disable-documentation --enable-mini-gmp --prefix=$$(pwd) libdir=$$(pwd)/lib CFLAGS="-O2 -ffunction-sections -fdata-sections"

clean:
	@[ -d $(prj) ] && ( cd $(prj); make clean; cd ..; rm -r $(prj) 2>/dev/null ) || true; \
	printf "Cleanup done.\n" 1>&2
	
build:
	$(MAKE) -C $(prj)

install:
	$(MAKE) -C $(prj) install
	cp -a $(prj)/lib/libnettle.a ../src/
