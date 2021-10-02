#!/bin/sh

set -ex

wget --continue http://downloads.openwrt.org/releases/17.01.7/targets/ar71xx/generic/lede-imagebuilder-17.01.7-ar71xx-generic.Linux-x86_64.tar.xz

tar -xf lede-imagebuilder-17.01.7-ar71xx-generic.Linux-x86_64.tar.xz
cd lede-imagebuilder-17.01.7-ar71xx-generic.Linux-x86_64

make -j4 image PROFILE=tl-mr3220-v1 PACKAGES=\
"-libiwinfo-lua -liblua -libubus-lua -libuci-lua -lua \
-luci -luci-app-firewall -luci-base -luci-lib-ip \
-luci-lib-nixio -luci-mod-admin-full -luci-proto-ipv6 \
-luci-proto-ppp -luci-theme-bootstrap -uhttpd -uhttpd-mod-ubus \
mosquitto-nossl"
