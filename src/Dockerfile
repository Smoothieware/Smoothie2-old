#
# To build, from base directory:
#
#   docker --tag shimaore/smoothie2 src/
#
FROM shimaore/smoothie2-gcc4mbed

COPY . /opt/smoothie/src
WORKDIR /opt/smoothie/src

RUN PATH=$PATH:/opt/smoothie/gcc4mbed/gcc-arm-none-eabi/bin make
