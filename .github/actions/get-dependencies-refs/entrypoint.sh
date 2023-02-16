#!/bin/sh -l

echo Here are the args: $*
echo "Cloning: git clone https://github.com/$1 Vulkan-Headers"
echo "A quick test" > test.txt
git clone https://github.com/$1 Vulkan-Headers
cd Vulkan-Headers
git checkout $2
echo "A quick test" > test.txt
mkdir build
cd build
cmake ..
DESTDIR=install make install
echo "Done installing headers"
echo "A quick test" > test.txt
