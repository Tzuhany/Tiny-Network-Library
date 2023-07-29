#!/bin/bash

set -e

if [ ! -d `pwd`/build ]; then
    sudo mkdir `pwd`/build
fi

sudo rm -rf `pwd`/build/*

cd `pwd`/build &&
    cmake .. &&
    make

cd ..

if [ ! -d /usr/include/zhnetlib ]; then
    sudo mkdir /usr/include/zhnetlib
fi

for header in `ls *.h`
do
    sudo cp $header /usr/include/zhnetlib
done

sudo cp `pwd`/lib/libzhnetlib.so /usr/lib

ldconfig