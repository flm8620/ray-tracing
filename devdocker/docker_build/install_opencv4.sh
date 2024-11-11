#!/bin/bash
set -e

tar xvf opencv-4.10.0.tar.gz

cd opencv-4.10.0
mkdir build
cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DWITH_IPP=OFF \
  -DBUILD_opencv_apps=OFF \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_TESTS=OFF \
  -DBUILD_SHARED_LIBS=OFF \
  -DENABLE_CXX11=ON \
  -DBUILD_JAVA=OFF \
  -DBUILD_TESTS=OFF \
  -DWITH_VTK=OFF \

make -j 8
sudo make install
