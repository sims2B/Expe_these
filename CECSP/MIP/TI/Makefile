#!/bin/make
# makefile for PROJECT shared library
# 2013-05-18

### BIN ###############################################

CC=g++
MKDIR=@mkdir -p
RM=@rm
CP=@cp

RELEASE_DIR=${HOME}/local
DEBUG=0
#enable to provide debug infos

### FLAGS #############################################
LDFLAGS= -L/home/mnattaf/Library/cplex_12_6/cplex/lib/x86-64_linux/static_pic -L/home/mnattaf/Library/cplex_12_6/concert/lib/x86-64_linux/static_pic
LDLIBS= -lconcert -lilocplex -lcplex -lpthread
CPPFLAGS= -I./header/ -I/home/mnattaf/Dev2016/CECSP/Problem/header/ -I/home/mnattaf/Dev2016/CECSP/ER/header/ -I/home/mnattaf/Dev2016/CECSP/Problem/source/ -I/home/mnattaf/Library/cplex_12_6/cplex/include -I/home/mnattaf/Library/cplex_12_6/concert/include -std=c++11 -O2 -m64 -fPIC -march=native -Wall  -DIL_STD 

DEBUG_FLAGS= -W  -Werror -Wall -gstabs


### PROPERTIES ########################################
PROJECTNAME=bin/modelTI
SRCS=~/Dev2016/CECSP/Problem/source/LinearFunction.cpp \
      ~/Dev2016/CECSP/Problem/source/Interval.cpp \
      ~/Dev2016/CECSP/Problem/source/Piece.cpp \
      ~/Dev2016/CECSP/Problem/source/Task.cpp \
      ~/Dev2016/CECSP/Problem/source/Problem.cpp \
      ~/Dev2016/CECSP/Problem/source/Solution.cpp \
	~/Dev2016/CECSP/MIP/TI/source/timeModel.cpp \
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
