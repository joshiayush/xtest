#!/bin/bash

echo '################################################'
echo 'Generating build system for xtest in "build/"...'
echo '################################################'
echo ''

cmake -B build/ -S .

echo ''
echo '##########################################'
echo 'Building object code into "libxtest.so"...'
echo '##########################################'
echo ''

cd build
make install

echo ''
echo '##########################################################'
echo 'Linking "./build/libxtest.so" to "/usr/lib/libxtest.so"...'
echo '##########################################################'
echo ''

sudo ln -s ./build/libxtest.so /usr/lib/libxtest.so

echo ''
echo '#####################################################'
echo 'Linking header files to "/usr/include/C++/9/xtest"...'
echo '#####################################################'
echo ''

if [ -d "/usr/include/C++/9/xtest" ]; then
    echo '"/usr/include/C++/9/xtest" directory exists!'
else
    echo '"/usr/include/C++/9/xtest" directory does not exists!'
    echo 'Making directory "/usr/include/C++/9/xtest"...'
    sudo mkdir /usr/include/C++/9/xtest
fi

sudo ln -s include/** /usr/include/C++/9/xtest
