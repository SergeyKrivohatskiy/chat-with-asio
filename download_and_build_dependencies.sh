#!/bin/bash
set -e
#protobuf
rm -rf protobuf
git clone https://github.com/google/protobuf.git
cd protobuf
git checkout v2.6.1 -b v2.6.1
./autogen.sh
mkdir -p install_out && ./configure --prefix=$PWD/install_out
make
make check
make install
cd ..

#boost
rm -rf boost
git clone --recursive https://github.com/boostorg/boost.git
cd boost
git checkout boost-1.59.0 -b boost-1.59.9
./bootstrap.sh --includedir=$PWD/install_out/include --libdir=/$PWD/install_out/lib
./bjam install
cp -R -f libs/asio/include -t install_out/include
cd ..
