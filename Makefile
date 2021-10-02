#PATH:=FIXME/openwrt/OpenWrt-Toolchain-ar71xx-generic_gcc-5.3.0_musl-1.1.16.Linux-x86_64/toolchain-mips_34kc_gcc-5.3.0_musl-1.1.16/bin:${PATH}
#PREFIX=mips-openwrt-linux

CC=${PREFIX}-gcc
STRIP=${PREFIX}-strip

CFLAGS=$(shell pkg-config --cflags libusb-1.0) 
CFLAGS+=-Os

LDFLAGS= libusb-1.0.24/libusb-1.0.a -Os -static



all: libusb-1.0.24/libusb-1.0.a pl2303gpio/cp2103.c
	${CC} ${CFLAGS} pl2303gpio/cp2103.c pl2303gpio/usb.c main.c $(LDFLAGS) -pthread
	cp a.out a.out.nost
	${STRIP} a.out
	wc a.out*
	cat a.out | bzip2 -9 > a.out.bz2
	#bzip2 -9 a.out
	#./a.out

pl2303gpio/cp2103.c:
	git submodule init
	git submodule update

libusb-1.0.24.tar.bz2:
	wget https://github.com/libusb/libusb/releases/download/v1.0.24/libusb-1.0.24.tar.bz2

libusb-1.0.24/libusb-1.0.a: libusb-1.0.24/configure
	cd libusb-1.0.24 && ./configure --disable-udev --enable-static --disable-shared
	cd libusb-1.0.24 && make
	mv libusb-1.0.24/libusb/.libs/libusb-1.0.a $@

libusb-1.0.24/configure: libusb-1.0.24.tar.bz2
	tar -xf libusb-1.0.24.tar.bz2

# For OpenWRT:
lede:
	#wget http://downloads.openwrt.org/releases/17.01.7/targets/ar71xx/generic/lede-imagebuilder-17.01.7-ar71xx-generic.Linux-x86_64.tar.xz
	#tar -xf lede-imagebuilder-17.01.7-ar71xx-generic.Linux-x86_64.tar.xz
	#cd lede-imagebuilder-17.01.7-ar71xx-generic.Linux-x86_64 && ../ledemake.sh
	./ledemake.sh

clean:
	rm -Rf libusb*
	rm -Rf lede-imagebuilder*
	rm -Rf a.out  a.out.bz2  a.out.nost
	rm -Rf pl2303gpio

