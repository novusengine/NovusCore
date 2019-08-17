FROM ubuntu:xenial

# Configuration prerequisites
RUN apt-get update \
    && apt-get install -y \
        software-properties-common \
        wget \
        apt-transport-https \
        ca-certificates \
        gnupg \
        gettext-base

# Setup GPG keys
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key 2>/dev/null \
        | apt-key add - \
    && wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null \
        | apt-key add -

# Setup PPAs and repos
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test \
    && apt-add-repository 'deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial main' \
    && apt-add-repository 'deb https://apt.kitware.com/ubuntu/ xenial main' \
    && apt-get update

# Common build utilities
RUN apt-get install -y \
        make \
        cmake \
        git \
        vim \
        gcc-8 \
        g++-8 \
        clang-6.0 \
        dos2unix

# Project-specific packages
RUN apt-get install -y \
        libmysqlclient-dev \
        libssl-dev \
        libbz2-dev

# Container user initialization
RUN useradd -ms /bin/bash docker

USER docker
WORKDIR /home/docker

# Entrypoint build script
COPY ./travisBuildScript.sh /tmp/travisBuildScript.sh
RUN mkdir -p /home/docker/novuscore
RUN dos2unix -n /tmp/travisBuildScript.sh /home/docker/travisBuildScript.sh
ENTRYPOINT ["/home/docker/travisBuildScript.sh"]
