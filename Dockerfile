#
# Usage:   docker --tag shimaore/smoothie2 .
#

FROM debian:testing
MAINTAINER stephane@shimaore.net

RUN \
  dpkg --add-architecture i386 && \
  apt-get update && \
  apt-get install -y --no-install-recommends \
    ca-certificates \
    lbzip2 \
    make \
    wget \
    libc6:i386 \
    libncurses5:i386 \
    libstdc++6:i386

COPY gcc4mbed/ /opt/smoothie/gcc4mbed
WORKDIR /opt/smoothie/gcc4mbed

RUN \
  yes | ./linux_install && \
  rm -rf external/mbed/Release

COPY src/ /opt/smoothie/src
WORKDIR /opt/smoothie/src

RUN PATH=$PATH:/opt/smoothie/gcc4mbed/gcc-arm-none-eabi/bin make
