HOME_TREE := ../..

# TODO: set the prj_name here
# default name is your folder's title
PRJ_NAME := $(notdir $(CURDIR))

# TODO: set the output binary name here
# default name is your folder's tilte
BINARY_NAME := $(notdir $(CURDIR))

# Keep the following line if you need GCC-AVR3.3 to generate code with smaller size
# NOTICE: it seems arduino no longer support GCC3.3 anymore...
# GCC_VERSION := 3.3
#

# enable the following marco in case you want these features
#EXTRA_DEFS = -D_ENABLE_SERIAL_CALLBACK
EXTRA_DEFS = -D_SERIAL_CUSTOM_RX_ROUTINE_

#includes the common definitions...
#do not remove them unless you know what you are doing
include $(HOME_TREE)/mak_def.inc
include $(HOME_TREE)/mak_avr_def.inc
#############################################
# !!!!
# !!!! the following are the final settings
# !!!! enable them when flashing to the final PCB !!!!
MCU := atmega88
F_CPU := 12000000
PORT_RATE := 19200

#enable the following line of code and modify the PORT value to the one you will use to download the firmware
UPLOAD_PORT := COM5

# the following are prototype board settings!,
# modify them if necessary...
#UPLOAD_PORT := COM4
PORT_RATE := 19200
#MCU := atmega8
#F_CPU := 8000000
#############################################


################
# BURNING CONF
#
LFUSE=CE
HFUSE=DD
EFUSE=00

LOCKFUSE=00

# TODO: list the C-Source files here
# All of the .c files within this folder will be included by default
CSRC := $(shell find src/ -name "*.c")

# TODO: list the CPP-Source files here
# All of the .cpp files within this folder will be included by default
CXXSRC := $(shell find src/ -name "*.cpp")

#some workaround to fix the __cxa_pure_virtual problem
#CXXSRC += cpp_workaround.cpp

# TODO: list the Assmebly-Source files here
# All of the .S files within this folder will be included by default
ASRC := $(shell find src/ -name "*.S")

# TODO: Put the header's path that you want to include
INCLUDE_PATH +=  -I$(SOURCE_ROOT)/infra


all:  build_avr_lib build sizebefore sizeafter

burnbootloader: 
ifeq ($(MCU),atmega88)
  ifeq ($(F_CPU),12000000)
	$(AVRDUDE) $(BURNING_FLAGS) -U flash:w:$(BOOTLOADER_USB_M88_12M)
  else
	@ECHO the usb bootloader only supports 12M clock rate
  endif
else
	@ECHO the bootloader cannot run on the chip $(MCU), please replace the bootloader file
endif

burnall: $(TARGET).hex erase fuse burnbootloader
	$(AVRDUDE) $(BURNING_FLAGS) -D $(BURNING_WRITE_FLASH)

burnrelease: burnall lock


#include the following file if you need standard avr runtime.
#(Currently, it is arduino)
include $(AVR_RUNTIME_PATH)/mak_lib_lit.inc

#include standard build logic
include $(HOME_TREE)/mak_avr_common.inc


additionalclean: clean_avr_lib
