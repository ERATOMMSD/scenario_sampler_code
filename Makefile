TARG=sss
BUILD_DIR=build

all: $(TARG)

RM=rm -rf
DIFF=diff --strip-trailing-cr -q
UNAME=$(shell uname)
TOUCH=touch
CFLAGS=-Wfatal-errors
LDFLAGS=
STRIP=:

OS := $(shell uname -s)

ifeq (,$(SSS_TARGPC))
	LD=g++
	ifeq (,$(shell which ccache 2> /dev/null))
		CC=gcc -c
		CXX=g++ -c
	else
		CC=ccache gcc -c
		CXX=ccache g++ -c
	endif
	ifeq ($(OS),Darwin)
		STRIP=strip
	else
		LDFLAGS+=-lrt
	endif
else
	LD=$(SSS_TARGPC)-mingw32-g++
	CC=$(SSS_TARGPC)-mingw32-gcc -c
	CXX=$(SSS_TARGPC)-mingw32-g++ -c
	LDFLAGS+=--static
endif

CXX_SRCS = util.cpp xml.cpp sss.cpp
C_SRCS=

OBJS+=$(CXX_SRCS:%.cpp=$(BUILD_DIR)/%.o) $(C_SRCS:%.c=$(BUILD_DIR)/%.o)
DEPENDS=$(OBJS:%.o=%.d)

CFLAGS+=-DSSS_LOG

#### build ####

$(TARG): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(TARG)
	$(STRIP) $(TARG)

relink:
	$(LD) $(OBJS) $(LDFLAGS) -o $(TARG)
	$(STRIP) $(TARG)


FORCE:

clean:
	$(RM) $(TARG) *.stackdump $(OBJS)

depend-clean:
	$(RM) $(DEPENDS)

.PHONY: clean depend-clean all FORCE relink

.SUFFIXES: .d .o .c .cpp

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.d: %.c
	$(CC) $(CFLAGS) -MM -MG -MT '$(@:%.d=%.o)' $< > $@

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.d: %.cpp
	$(CXX) $(CFLAGS) -MM -MG -MT '$(@:%.d=%.o)' $< > $@

minisat22-2/%.o: minisat22-2/%.cc
	$(CXX) -std=gnu++11 $(CFLAGS) -o $@ $<

-include $(DEPENDS)
