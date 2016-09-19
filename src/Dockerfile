#
# To build, from base directory:
#
#   docker build --tag shimaore/smoothie2 src/
#
# To export the binary:
#
#   docker run shimaore/smoothie2 /bin/sh -c 'tar cf - LPC4330_M4/Smoothie2*' | tar xvf -
#
FROM shimaore/smoothie2-gcc4mbed

COPY . /opt/smoothie/src
WORKDIR /opt/smoothie/src

RUN PATH=$PATH:/opt/smoothie/gcc4mbed/gcc-arm-none-eabi/bin make
