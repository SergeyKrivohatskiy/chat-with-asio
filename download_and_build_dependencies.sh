#!/bin/bash
#protobuf
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
git clone --recursive https://github.com/boostorg/boost.git
cd boost
git checkout boost-1.59.0 -b boost-1.59.9
./bootstrap.sh
./bjam install --prefix=$PWD/install_out
cd ..
