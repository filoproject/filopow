#!/usr/bin/env bash

export LC_ALL=C

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/.. || exit

DOCKER_IMAGE=${DOCKER_IMAGE:-The-Filopow-Endeavor/filopowd-develop}
DOCKER_TAG=${DOCKER_TAG:-latest}

BUILD_DIR=${BUILD_DIR:-.}

rm docker/bin/*
mkdir docker/bin
cp $BUILD_DIR/src/filopowd docker/bin/
cp $BUILD_DIR/src/filopow-cli docker/bin/
cp $BUILD_DIR/src/filopow-tx docker/bin/
strip docker/bin/filopowd
strip docker/bin/filopow-cli
strip docker/bin/filopow-tx

docker build --pull -t $DOCKER_IMAGE:$DOCKER_TAG -f docker/Dockerfile docker
