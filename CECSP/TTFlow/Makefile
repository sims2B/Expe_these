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
LDFLAGS= -L/home/mnattaf/Library/cplex_216/cplex/lib/x86-64_linux/static_pic -L/home/mnattaf/Library/cplex_216/concert/lib/x86-64_linux/static_pic
LDLIBS= -lconcert -lilocplex -lcplex -lpthread
CPPFLAGS= -I/home/mnattaf/Dev2016/CECSP/TTDR/header/ -I/home/mnattaf/Dev2016/CECSP/Problem/source/ -I/home/mnattaf/Dev2016/CECSP/Problem/header/   -I./header -I/home/mnattaf/Library/cplex_216/cplex/include -I/home/mnattaf/Library/cplex_216/concert/include -std=c++11 -O2 -m64 -fPIC -march=native -Wall  -DIL_STD 
DEBUG_FLAGS= -W  -Werror -Wall -g

### PROPERTIES ########################################
PROJECTNAME=bin/model_flot
SRCS=	/home/mnattaf/Dev2016/CECSP/Problem/source/LinearFunction.cpp \
	/home/mnattaf/Dev2016/CECSP/Problem/source/Interval.cpp \
	/home/mnattaf/Dev2016/CECSP/Problem/source/Piece.cpp \
	/home/mnattaf/Dev2016/CECSP/Problem/source/Task.cpp \
	/home/mnattaf/Dev2016/CECSP/Problem/source/Problem.cpp \
	/home/mnattaf/Dev2016/CECSP/TTDR/source/ttdr.cpp \
	./source/flot_model.cpp \
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

