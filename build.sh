#!/bin/bash

set -e

IMAGE=gcc
IMAGE_NAME=build
TAG=`if [ -z "$2" ]; then echo "master"; else echo "$2" ; fi`
PID=$(sudo docker run -itd -v $(pwd):/repo -v $(pwd)/.build:/build spielhuus/toolchain /bin/sh)
echo "build lightningcpp tag:$TAG, image:$PID"

DOCKER_EXEC="sudo docker exec $PID /bin/sh -c"

$DOCKER_EXEC "cd build && conan install /repo --build=missing -s compiler.libcxx=libstdc++11"
$DOCKER_EXEC "cmake -H/repo -B/build -G Ninja -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja -Dbuild_tests=on -Dbuild_example_server=on -Dbuild_documentation=on -DLIGHTNING_TAG_VERSION=$TAG"
$DOCKER_EXEC "cmake --build /build"
$DOCKER_EXEC "cmake --build /build --target test"
$DOCKER_EXEC "cmake --build /build --target doc"
$DOCKER_EXEC "cmake --build /build --target package"

if [ -z "$1" ]; then
	echo not deploying to bintray, set api key to do so.
else
	$DOCKER_EXEC "cd /build/conan && conan export conan-cpp/latest"
	$DOCKER_EXEC "conan install lightningcpp/$TAG@conan-cpp/latest --build=lightningcpp"
	$DOCKER_EXEC "conan user -p $1 -r conan-cpp squawkcpp"
	$DOCKER_EXEC "conan upload lightningcpp/$TAG@conan-cpp/latest --all -r=conan-cpp"
fi

sudo docker build -f docker/Dockerfile --build-arg LIGHTNING_TAG_VERSION=$TAG -t lightning .

sudo rm -rf .build
sudo docker rm -f $PID
