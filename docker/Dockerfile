FROM debian:stretch
LABEL maintainer="Filopow Developers <admin@github.com/filoproject/filopow>"
LABEL description="Dockerised FilopowCore, built from Travis"

RUN apt-get update && apt-get -y upgrade && apt-get clean && rm -fr /var/cache/apt/*

COPY bin/* /usr/bin/
