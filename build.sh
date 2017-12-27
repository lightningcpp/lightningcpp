#!/bin/bash

set -e

sudo docker pull spielhuus/clang:latest
sudo docker run -itd --name build_lightning -v $(pwd):/repo -v $(pwd)/build:/build spielhuus/clang

sudo docker exec build_lightning cmake -H/repo -B/build -G Ninja -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja -Dbuild_tests=on -Dbuild_example_server=on -Dbuild_documentation=on
sudo docker exec build_lightning cmake --build /build
sudo docker exec build_lightning cmake --build /build --target test
sudo docker exec build_lightning cmake --build /build --target doc
sudo docker exec build_lightning cmake --build /build --target package

sudo docker build -f docker/Dockerfile -t lightning .

sudo docker rm -f build_lightning
sudo rm -rf build



