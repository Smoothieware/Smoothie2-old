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
  rm -r \
    external/mbed/Release \
    external/mbed/libraries/mbed/targets/cmsis/TARGET_ARM_SSG \
    external/mbed/libraries/mbed/targets/cmsis/TARGET_Atmel \
    external/mbed/libraries/mbed/targets/cmsis/TARGET_Freescale \
    external/mbed/libraries/mbed/targets/cmsis/TARGET_Maxim \
    external/mbed/libraries/mbed/targets/cmsis/TARGET_NORDIC \
    external/mbed/libraries/mbed/targets/cmsis/TARGET_RENESAS \
    external/mbed/libraries/mbed/targets/cmsis/TARGET_STM \
    external/mbed/libraries/mbed/targets/cmsis/TARGET_Silicon_Labs \
    external/mbed/libraries/mbed/targets/cmsis/TARGET_WIZNET \
    external/mbed/libraries/mbed/targets/cmsis/TOOLCHAIN_IAR \
    external/mbed/libraries/mbed/targets/hal/TARGET_ARM_SSG \
    external/mbed/libraries/mbed/targets/hal/TARGET_Atmel \
    external/mbed/libraries/mbed/targets/hal/TARGET_Freescale \
    external/mbed/libraries/mbed/targets/hal/TARGET_Maxim \
    external/mbed/libraries/mbed/targets/hal/TARGET_NORDIC \
    external/mbed/libraries/mbed/targets/hal/TARGET_RENESAS \
    external/mbed/libraries/mbed/targets/hal/TARGET_STM \
    external/mbed/libraries/mbed/targets/hal/TARGET_Silicon_Labs \
    external/mbed/libraries/mbed/targets/hal/TARGET_WIZNET

COPY src/ /opt/smoothie/src
WORKDIR /opt/smoothie/src

RUN PATH=$PATH:/opt/smoothie/gcc4mbed/gcc-arm-none-eabi/bin make
