#!/bin/make
# makefile for PROJECT shared library
# 2013-05-18

### BIN ###############################################

CC=g++
MKDIR=@mkdir -p
RM=@rm
CP=@cp

RELEASE_DIR=${HOME}/local
DEBUG=0#enable to provide debug infos

### FLAGS #############################################
LDFLAGS=  -L/home/mnattaf/Library/lib
LDLIBS=  -lrt -lCGAL -lboost_thread -lboost_system -lgmp -lmpfr -lm -lpthread -lrt
CPPFLAGS= -I/home/mnattaf/Library/include -I./header  -I/home/mnattaf/Dev2016/CECSP/Problem/header -I/home/mnattaf/Dev2016/CECSP/Problem/source/ -std=c++11 -O2 -m64 -fPIC  -Wall -march=native
DEBUG_FLAGS= -W  -Werror -Wall -g

### PROPERTIES ########################################
PROJECTNAME=bin/ER
SRCS= ~/Dev2016/CECSP/Problem/source/LinearFunction.cpp \
      ~/Dev2016/CECSP/Problem/source/Interval.cpp \
      ~/Dev2016/CECSP/Problem/source/Piece.cpp \
      ~/Dev2016/CECSP/Problem/source/Task.cpp \
      ~/Dev2016/CECSP/Problem/source/Problem.cpp \
      ~/Dev2016/CECSP/Problem/source/Solution.cpp \
	./source/ListInterval.cpp \
       ./source/main.cpp


### PREPROCESSING #####################################

ifeq (${DEBUG},1)
	CPPFLAGS+=${DEBUG_FLAGS}
endif

OBJS=$(subst .cpp,.o,$(SRCS))

### TARGETS ###########################################

all: $(PROJECTNAME)

depend: .depend

.depend: $(SRCS)
	@rm -f .depend
	$(CC) $(CPPFLAGS) -MM $^>>./.depend

$(PROJECTNAME): pre $(OBJS)
	$(CC) $(LDFLAGS) -o $(PROJECTNAME) $(OBJS) $(LDLIBS)

pre:
	$(MKDIR) bin

install: $(PROJECTNAME)
	$(CP) $(PROJECTNAME) $(RELEASE_DIR)/lib/$(PROJECTNAME)
	$(CP) -Ru includes/* $(RELEASE_DIR)/lib/includes/

mrproper: clean
	$(RM) -rf bin/* $(PROJECTNAME)

clean:
	$(RM) -f $(OBJS) .depend

