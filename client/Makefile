ifndef CFG
CFG := Debug
endif

ifeq (${CFG}, Debug)
CPPFLAGS += -g -O0 -D_DEBUG -Iinclude -I../utils -Icommon
LDFLAGS += -g
else ifeq (${CFG}, Release)
CPPFLAGS += -DNDEBUG -Iinclude -I../utils -Icommon
#LDFLAGS += -O
else
$(error Invalid CFG specified: ${CFG})
endif
$(info Using CFG: ${CFG})

# Compile with GCC v6.1.0
# COMPILER_GCC_610 := /ray/links/apps/gcc/versions/6.1.0/g++
# COMPILER_GCC_610 := /usr/local/gcc-6.1.0/bin/g++
CXX := x86_64-w64-mingw32-gcc
# LD_LIBRARY_PATH := /usr/local/gcc-6.1.0/lib64:${LD_LIBRARY_PATH}

###############################################################################
DSTDIR := ${CFG}

CXXFLAGS += -Wall

.PHONY: default all clean cleanall test

default: all

# This contains all the paths and executables we want to build?
all: ${DSTDIR} ${DSTDIR}/client

clean:
	@echo
	@echo "Cleaning ..."
	${RM} -r ${DSTDIR}

cleanall: clean
	@echo
	@echo "Cleaning ALL ..."
	${RM} *~ testFile.dat
	${RM} -r rpt

${DSTDIR}/%.o: %.c
	@echo
	@echo "Compiling $@ ..."
	mkdir -p $(dir $@)
	${CXX} ${CXXFLAGS} ${CPPFLAGS} -c $< -o $@

# Get list of all CPPs in current directory
SRC_DIR := src
CPPS := $(wildcard $(SRC_DIR)/**/*.c $(SRC_DIR)/*.c)
$(info CPPS: ${CPPS})
# Compute the list of OBJS from the list of CPPS
OBJS := $(patsubst %.c, ${DSTDIR}/%.o, ${CPPS})
$(info OBJS: ${OBJS})


# Hello is some executable... make multiple of these if there's multiple executatbles?
${DSTDIR}/client : ${OBJS}
	@echo
	@echo "Linking $@ ..."
	$(CXX) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

${DSTDIR} :
	@echo
	@echo "mkdir $@ ..."
	mkdir $@