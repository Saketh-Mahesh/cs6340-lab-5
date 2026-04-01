FROM --platform=linux/amd64 ubuntu:focal-20220105
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC
RUN yes | unminimize
RUN apt-get update && apt-get install -y \
    clang-8 \
    clang-format-8 \
    cmake \
    curl \
    git \
    g++ \
    gdb \
    libz3-dev \
    llvm-8-dev \
    llvm-8-tools \
    man-db \
    manpages-dev \
    manpages-posix \
    manpages-posix-dev \
    nano \
    unzip \
    vim \
    wget \
    zlib1g-dev \
    && ln -s /usr/bin/clang-8 /usr/bin/clang \
    && ln -s /usr/bin/clang++-8 /usr/bin/clang++ \
    && ln -s /usr/bin/opt-8 /usr/bin/opt \
    && ln -s /usr/bin/clang-format-8 /usr/local/bin/clang-format
RUN git clone https://github.com/jeaye/stdman \
    && cd stdman \
    && ./configure \
    && make install \
    && cd .. \
    && rm -rf stdman
RUN curl -fsSL https://nodejs.org/dist/v10.24.1/node-v10.24.1-linux-x64.tar.xz | tar -xJ -C /opt \
    && ln -s /opt/node-v10.24.1-linux-x64 /opt/node \
    && ln -s /opt/node/bin/node /usr/local/bin/node \
    && ln -s /opt/node/bin/npm /usr/local/bin/npm \
    && npm install -g typescript@3.8.3
RUN cd /opt \
    && curl -sL https://github.com/adoptium/temurin8-binaries/releases/download/jdk8u302-b08/OpenJDK8U-jdk_x64_linux_hotspot_8u302b08.tar.gz | tar -xz \
    && ln -s /opt/jdk8u302-b08 /opt/jdk8
ENV PATH=/opt/jdk8/bin:$PATH
ENV JAVA_TOOL_OPTIONS="-Dfile.encoding=UTF-8"
RUN useradd cs6340 -s /bin/bash -U -m
USER cs6340
WORKDIR /home/cs6340
