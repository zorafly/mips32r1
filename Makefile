#Copyright 2019 April May <zorafly@icloud.com>

# *********************** #
# Configuration variables #
# *********************** #

#Features to enable (see features.txt)
FEATURES := 
#C standard version to target 
STD := c11
# Location of gcc or clang compiler to use
CC := gcc

# ****************************************************************** #
# The following variables are NOT intended for general configuration #
# ****************************************************************** #

# Header include directories
INCLUDES := ./include/public ./include/private /usr/local/include
# Libraries to link against
LIBS := m
# Common source files for all targets
SRC := mem.c mips32r1.c cpu.c fileutil.c debug.c
# Final output path for shared library
LIB_OUT := libmips32r1.so
# Final output path for executable
BIN_OUT := mips32r1
# Directory of source files
SRC_DIR = src
# Directory for intermediate object output
OBJ_DIR := build/obj
# Directory for GCC dependency output
DEP_DIR := build/dep
# Linker options
LOPTS := --version-script=mips32r1.map
#Compute list of feature macros
FEATURES_ENABLED := $(patsubst %,-D%_ENABLED,${FEATURES}) -masm=intel
#Compiler flags for default mode
CFLAGS_DEFAULT := -Og -ggdb -mtune=generic ${FEATURES_ENABLED} -ftrapv -fstack-protector-strong -fcf-protection=full -fstack-clash-protection -std=${STD} -DDEBUG_ENABLED=0 -Wall -Wextra -Werror -pedantic -pedantic-errors -Wno-unused-parameter
#Compiler flags for fast mode	
CFLAGS_FAST := -O3 -g0 -march=native ${FEATURES_ENABLED} -std=${STD} -DDEBUG_ENABLED=0 -fweb -funroll-loops
#Compiler flags for small mode
CFLAGS_SMALL := -Os -g0 ${FEATURES_ENABLED} -std=${STD} -DDEBUG_ENABLED=0
#Compiler flags for static mode
CFLAGS_STATIC := -Os -g0 -static -Wl,-static ${FEATURES_ENABLED} -std=${STD} -DDEBUG_ENABLED=0
#Compiler flags for debug mode
CFLAGS_DEBUG := -O0 -ggdb -Wextra -Wall -pedantic -pedantic-errors -Werror \
	-Wno-switch -ftrapv \
	-fsanitize=leak -fsanitize=pointer-compare -fsanitize=pointer-subtract\
	-fsanitize=address -fsanitize-address-use-after-scope \
	-fsanitize=pointer-overflow -fsanitize=undefined \
	-fstack-protector-strong -fcf-protection=full \
	-fstack-clash-protection ${FEATURES_ENABLED} \
	-std=${STD} -fanalyzer -Wno-analyzer-file-leak \
	-Wno-analyzer-null-argument -Wno-unused-parameter
#Compiler flag selection
ifneq (${fast},)
	CFLAGS := ${CFLAGS_FAST}
else ifneq (${small},)
	CFLAGS := ${CFLAGS_SMALL}
else ifneq (${static},)
	CFLAGS := ${CFLAGS_STATIC}
else ifneq (${dev},)
	CFLAGS := ${CFLAGS_DEBUG} 
else
	CFLAGS := ${CFLAGS_DEFAULT}
endif
#Add header includes to CFLAGS
INCLUDES := ${foreach inc,${INCLUDES},-I${inc}}
CFLAGS := ${INCLUDES} ${CFLAGS}
#Common object files and dependencies for all targets
CFILES := ${filter %.c,${SRC}}
OBJ := ${foreach src,${CFILES},${OBJ_DIR}/${patsubst %.c,%.o,${src}}}
DEP := ${foreach src,${CFILES},${DEP_DIR}/${patsubst %.c,%.d,${src}}}
#Linker flags
LDFLAGS := -Llib/ ${LDFLAGS} ${foreach lib,${LIBS},-l${lib}} \
	${foreach opt,${LOPTS},-Wl,${opt}}

# ************** #
# Build targets  #
# ************** #

# This pseudo-target lists the names of all valid targets.
# .PHONY is not intended to be executed as a target. It serves to inform
# Make of the valid target names so that they do not clash with any files
# in the current working director.
.PHONY : all clean prepare shared bin

ifneq (${shell test -e build}},)	
	-include ${DEP}
endif

# The default target builds the shared library with default compiler flags.
# It also builds the runvxf tool with default compiler flags.
all: prepare shared bin

clean: 
	rm -rf build/ ${OUT}

prepare:
	@mkdir -p build/dep build/obj

shared: prepare ${DEP} ${OBJ} 
	@echo Linking shared library
	@${CC} ${CFLAGS} -flto -fwhole-program -shared -o ${LIB_OUT} ${OBJ} ${LDFLAGS}
bin: prepare ${DEP} ${OBJ}
	@echo Linking binary
	@${CC} ${CFLAGS} src/test.c -o ${BIN_OUT} ${OBJ} ${LDFLAGS}

${DEP_DIR}/%.d: ${SRC_DIR}/%.c 
	@echo Building dependencies for $<
	@${CC} ${CFLAGS} -MM -MF $@ -MT ${OBJ_DIR}/${patsubst %.c,%.o,${notdir $<}} -c $<

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c  
	@echo Compiling $<
	@${CC} ${CFLAGS} -c -fPIC $< -o $@






