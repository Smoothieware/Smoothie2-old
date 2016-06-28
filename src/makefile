# Copyright 2015 Adam Green (http://mbed.org/users/AdamGreen/)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
PROJECT            := Smoothie2
DEVICES            := LPC4330_M4
GCC4MBED_DIR       := ../gcc4mbed/
NO_FLOAT_SCANF     := 1
NO_FLOAT_PRINTF    := 0
LPC4330_M4_LSCRIPT := LPC4337.ld
LIBS_PREFIX        := configdefault.o
OPTIMIZATION       =  0

# Use C++11 features for the checksums
DEFINES += -DCHECKSUM_USE_CPP

include $(GCC4MBED_DIR)/build/gcc4mbed.mk

configdefault.o : config.default
	@echo Packing $<
	$(Q) $(OBJCOPY) -I binary -O elf32-littlearm -B arm --readonly-text --rename-section .data=.rodata.configdefault $< $@

flash:
	lpc21isp LPC4330_M4/Smoothie2.hex /dev/ttyUSB0 115200 14746
