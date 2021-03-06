#*************************************************************************\
#  Copyright (c) 2007 by China Chip Tech.                                 *
#                                                                         *
#   PROJECT     : ccpmp                                                   *
#                                                                         *
#   MODULE      : MakeFile                                                *
#                                                                         *
#   PROCESSOR   : MIPS 4Kc (32 bit RISC) - 4730 board                     *
#                                                                         *
#   Tool-chain  : mipsel-linux-gcc                                        *
#                                                                         *
#   DESCRIPTION :                                                         *
#   Makefile used for building application.                               *
#                                                                         *
#   The default target (all) builds application in two formats :          *
#   *.elf : Image in ELF format.                                          *
#   *.bin : Image in BIN format.                                          *
#                                                                         *
#   The linker also generates a MAP file "*.map".                         *
#                                                                         *
#   Other targets are :                                                   *
#   clean :    Deletes all files generated by makefile.                   *
#   depend :   Builds dependency file.                                    *
#                                                                         *
#  History :                                                              *
#    Version      Date      Author     Memo                               *
#     0.1     2007-05-22   fanoble    Created.                            *
#     0.2     2007-11-17   vrix.yan   Modify                              *
#     0.3     2007-11-22   wlean      Add down env                        *
#*************************************************************************/

CPP       = mipsel-linux-g++
CC        = mipsel-linux-gcc
LD        = mipsel-linux-ld
OBJCOPY   = mipsel-linux-objcopy
OBJDUMP   = mipsel-linux-objdump

# **********************************************
# Endianness
# **********************************************

ENDIAN  = EL
OFORMAT = elf32-littlemips

# **********************************************
# Name of application
# **********************************************

TARGET = downenv
LD_SCRIPT = link.xn
TEXT_BASE = 0x80a00000

# **********************************************
# Directories
# **********************************************

SRCDIR    = ../src main
VPATH     = $(SRCDIR)
INCDIR    = 
LIBROOT   = ../lib

# **********************************************
# Image file names and map file
# **********************************************

IMAGE_ELF = $(TARGET).elf
IMAGE_BIN = $(TARGET).bin
MAP       = $(TARGET).map

# **********************************************
# Compiler and linker options
# **********************************************

INCLUDE   = -I$(INCDIR) -I$(MIPSTOOLS) -I$

W_OPTS    = -Wall -finline-functions -fomit-frame-pointer -msoft-float \
-fno-builtin -fno-exceptions

CC_OPTS   = -g -c -O3 $(INCLUDE) -mips32 $(W_OPTS) -mno-abicalls -fno-pic \
-pipe -G 0 -D_DEBUG -DMPU_JZ4740

CC_OPTS_A = $(CC_OPTS) -D_ASSEMBLER_

CPP_OPTS  = -g -c -O3 $(INCLUDE) -mips32 $(W_OPTS) -mno-abicalls -fno-pic \
-pipe -G 0 -D_DEBUG -DMPU_JZ4740

LD_OPTS   = -Bstatic -g -T $(LD_SCRIPT) -o $(IMAGE_ELF) -Map $(MAP) \
-Ttext $(TEXT_BASE) -L$(LIBROOT) -L$(MIPSLIB)  -lm -lstdc++ -lc -lgcc -lmem 
#-Ttext $(TEXT_BASE) -L../lib -lm -lstdc++ -lc -lgcc

# **********************************************
# Files to be compiled
# **********************************************

SRC_C   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
SRC_CPP = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
SRC_S   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.S))
SRC     = $(SRC_C) $(SRC_CPP) $(SRC_S)

OBJ_C   = $(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJ_CPP = $(notdir $(patsubst %.cpp, %.o, $(SRC_CPP)))
OBJ_S   = $(notdir $(patsubst %.S, %.o, $(SRC_S)))
OBJ     = $(OBJ_C) $(OBJ_CPP) $(OBJ_S)

LIBS    = $(LIBROOT)/entry.a

# **********************************************
# Rules
# **********************************************

.PHONY : all

all : $(IMAGE_BIN)

$(IMAGE_BIN) : $(IMAGE_ELF)
	$(OBJCOPY) -O binary -R .export_table -R .export_string -R \
	.import_table -R .import_string -R .section_info $(IMAGE_ELF) $(IMAGE_BIN)
	$(OBJCOPY) -O binary -j .export_table $(IMAGE_ELF) export.tab
	$(OBJCOPY) -O binary -j .export_string $(IMAGE_ELF) export.str
	$(OBJCOPY) -O binary -j .import_table $(IMAGE_ELF) import.tab
	$(OBJCOPY) -O binary -j .import_string $(IMAGE_ELF) import.str
	$(OBJCOPY) -O binary -j .section_info $(IMAGE_ELF) section.dsc

$(IMAGE_ELF) : $(OBJ) $(LD_SCRIPT) 
	$(LD) $(OBJ) $(LIBS) $(LD_OPTS)

$(OBJ_C) : %.o : %.c
	$(CC) $(CC_OPTS) -o $@ $<

$(OBJ_CPP) : %.o : %.cpp
	$(CPP) $(CPP_OPTS) -o $@ $<

$(OBJ_S) : %.o : %.S
	$(CC) $(CC_OPTS_A) -o $@ $<

.PHONY : clean

clean :
	rm -f $(IMAGE_BIN) $(IMAGE_ELF) $(OBJ) $(MAP)
#	rm -f $(IMAGE_BIN) $(IMAGE_ELF) $(OBJ) $(MAP) *.dl *.tab *.str *.dsc

#**************************************************************************
#                              END  OF  FILE
#**************************************************************************
