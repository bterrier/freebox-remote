#!/bin/bash

set -x
set -e
BUILD_DIR=$PWD/build
export PKG_CONFIG_PATH=$BUILD_DIR/lib/pkgconfig

function build {
    libtoolize
    aclocal
    autoheader
    automake --force-missing --add-missing
    autoconf
    ./configure --prefix=$BUILD_DIR
    make -j4
    make install
}


cd libela
build 
cd ../librudp
build
cd ../foils_hid
build