PROJECT            := Smoothie2
DEVICES            := LPC4330_M4
GCC4MBED_DIR       := ../gcc4mbed/
NO_FLOAT_SCANF     := 1
NO_FLOAT_PRINTF    := 0
LIBS_PREFIX        := configdefault.o
OPTIMIZATION       =  0

# Use C++11 features for the checksums
DEFINES += -DCHECKSUM_USE_CPP


# Select the board that you want to build Smoothie to run upon.
# Can be one of: Smoothie2 (**default**)
#                Bambino210E
#                Bambino200E
BOARD ?= Smoothie2

# Need to use a custom LPC4337 script for boards like the Smoothie2 hardware which use that chip instead of the LPC4330.
ifeq "$(BOARD)" "Smoothie2"
LPC4330_M4_LSCRIPT := LPC4337.ld
endif

# Select the UART to be used for primary and secondary console (if any).
ifeq "$(BOARD)" "Smoothie2"
DEFINES += -DSMOOTHIE_UART_PRIMARY_ENABLE=1
DEFINES += -DSMOOTHIE_UART_PRIMARY_TX=P3_4
DEFINES += -DSMOOTHIE_UART_PRIMARY_RX=P3_5
DEFINES += -DSMOOTHIE_UART_SECONDARY_ENABLE=1
DEFINES += -DSMOOTHIE_UART_SECONDARY_TX=P2_0
DEFINES += -DSMOOTHIE_UART_SECONDARY_RX=P2_1
endif

ifeq "$(BOARD)" "Bambino210E"
DEFINES += -DSMOOTHIE_UART_PRIMARY_ENABLE=1
DEFINES += -DSMOOTHIE_UART_PRIMARY_TX=P2_10
DEFINES += -DSMOOTHIE_UART_PRIMARY_RX=P2_11
DEFINES += -DSMOOTHIE_UART_SECONDARY_ENABLE=1
DEFINES += -DSMOOTHIE_UART_SECONDARY_TX=P6_4
DEFINES += -DSMOOTHIE_UART_SECONDARY_RX=P6_5
endif

ifeq "$(BOARD)" "Bambino200E"
DEFINES += -DSMOOTHIE_UART_PRIMARY_ENABLE=1
DEFINES += -DSMOOTHIE_UART_PRIMARY_TX=P6_4
DEFINES += -DSMOOTHIE_UART_PRIMARY_RX=P6_5
DEFINES += -DSMOOTHIE_UART_SECONDARY_ENABLE=1
DEFINES += -DSMOOTHIE_UART_SECONDARY_TX=P5_6
DEFINES += -DSMOOTHIE_UART_SECONDARY_RX=P1_14
endif


include $(GCC4MBED_DIR)/build/gcc4mbed.mk


configdefault.o : config.default
	@echo Packing $<
	$(Q) $(OBJCOPY) -I binary -O elf32-littlearm -B arm --readonly-text --rename-section .data=.rodata.configdefault $< $@

flash:
	lpc21isp LPC4330_M4/Smoothie2.hex /dev/ttyUSB0 115200 14746
