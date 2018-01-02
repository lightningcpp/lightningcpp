#!/bin/bash

set -e

sudo docker run -it -v $(pwd):/repo -v $(pwd)/.build:/build spielhuus/toolchain /bin/bash -c 'cd /build && \
	conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan && \
	conan remote add conan-cpp https://api.bintray.com/conan/squawkcpp/conan-cpp && \
	conan install /repo --build=missing -s compiler.libcxx=libstdc++11 && \
	cmake -H/repo -B/build -G Ninja -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja -Dbuild_tests=on -Dbuild_example_server=on -Dbuild_documentation=on && \
	cmake --build /build && \
	cmake --build /build --target test && \
	cmake --build /build --target doc && \
	cmake --build /build --target package && \
	cd /repo/conan && conan export conan-cpp/latest

#	conan install lightningcpp/master@conan-cpp/latest --build=lightningcpp && \
#	conan user -p $BINTRAY_KEY -r conan-cpp squawkcpp && \
#	conan upload lightningcpp/master@conan-cpp/latest --all -r=conan-cpp'

sudo docker build -f docker/Dockerfile -t lightning .

sudo rm -rf .build
