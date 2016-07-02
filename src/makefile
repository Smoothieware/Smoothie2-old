# Select the board that you want to build Smoothie to run upon.
# Can be one of: Smoothie2 (**default**)
#                Bambino210E
#                Bambino200E
BOARD ?= Smoothie2

# Set MRI_ENABLE to a value of 1 to enable the MRI debug monitor on the secondary UART.
MRI_ENABLE ?= 0

# Set MRI_BREAK_ON_INIT to a value of 0 if you don't want MRI to break before running global constructors.
MRI_BREAK_ON_INIT ?= 1



########################################################################################################################
PROJECT             := Smoothie2
DEVICES             := LPC4330_M4
GCC4MBED_DIR        := ../gcc4mbed/
NO_FLOAT_SCANF      := 1
NO_FLOAT_PRINTF     := 0
LIBS_PREFIX         := configdefault.o
OPTIMIZATION        :=  0
MRI_SEMIHOST_STDIO  ?= 0

# Use C++11 features for the checksums
DEFINES += -DCHECKSUM_USE_CPP



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
MRI_UART ?= MRI_UART_TX_P2_0 MRI_UART_RX_P2_1 MRI_UART_BAUD=230400
endif

ifeq "$(BOARD)" "Bambino210E"
DEFINES += -DSMOOTHIE_UART_PRIMARY_ENABLE=1
DEFINES += -DSMOOTHIE_UART_PRIMARY_TX=P2_10
DEFINES += -DSMOOTHIE_UART_PRIMARY_RX=P2_11
DEFINES += -DSMOOTHIE_UART_SECONDARY_ENABLE=1
DEFINES += -DSMOOTHIE_UART_SECONDARY_TX=P6_4
DEFINES += -DSMOOTHIE_UART_SECONDARY_RX=P6_5
MRI_UART ?= MRI_UART_TX_P6_4 MRI_UART_RX_P6_5 MRI_UART_BAUD=230400
endif

ifeq "$(BOARD)" "Bambino200E"
DEFINES += -DSMOOTHIE_UART_PRIMARY_ENABLE=1
DEFINES += -DSMOOTHIE_UART_PRIMARY_TX=P6_4
DEFINES += -DSMOOTHIE_UART_PRIMARY_RX=P6_5
DEFINES += -DSMOOTHIE_UART_SECONDARY_ENABLE=1
DEFINES += -DSMOOTHIE_UART_SECONDARY_TX=P5_6
DEFINES += -DSMOOTHIE_UART_SECONDARY_RX=P1_14
MRI_UART ?= MRI_UART_TX_P5_6 MRI_UART_RX_P1_14 MRI_UART_BAUD=230400
endif

# Disable the secondary console if the MRI debug monitor is enabled so that the secondary UART can be used by
# MRI instead.
ifeq "$(MRI_ENABLE)" "1"
DEFINES       := $(subst -DSMOOTHIE_UART_SECONDARY_ENABLE=1,-DSMOOTHIE_UART_SECONDARY_ENABLE=0,$(DEFINES))
GCC4MBED_TYPE := Checked
else
GCC4MBED_TYPE := Release
endif


include $(GCC4MBED_DIR)/build/gcc4mbed.mk


configdefault.o : config.default
	@echo Packing $<
	$(Q) $(OBJCOPY) -I binary -O elf32-littlearm -B arm --readonly-text --rename-section .data=.rodata.configdefault $< $@

flash:
	lpc21isp LPC4330_M4/Smoothie2.hex /dev/ttyUSB0 115200 14746
