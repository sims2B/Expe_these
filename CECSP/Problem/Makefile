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
LDFLAGS=  -L/home/mnattaf/Library/lib -L/home/mnattaf/Library/CPLEX_128/cplex/lib/x86-64_linux/static_pic -L/home/mnattaf/Library/CPLEX_128/concert/lib/x86-64_linux/static_pic
LDLIBS=  -lconcert -lilocplex -lcplex  -lm -lpthread -lrt -ldl
CPPFLAGS= -I/home/mnattaf/Library/include -I./header -I./source -I../ER/header/  -I/home/mnattaf/Library/CPLEX_128/cplex/include -I/home/mnattaf/Library/CPLEX_128/concert/include  -std=c++11 -O2 -m64 -W  -Wall -Werror -fPIC -march=native -DIL_STD 
DEBUG_FLAGS= -W  -Wall -Werror -g 

### PROPERTIES ########################################
PROJECTNAME=bin/le_nom
SRCS=/home/mnattaf/Dev2016/CECSP/Problem/source/Task.cpp \
	/home/mnattaf/Dev2016/CECSP/Problem/source/Problem.cpp \
	/home/mnattaf/Dev2016/CECSP/Problem/source/Solution.cpp \
	/home/mnattaf/Dev2016/CECSP/ER/source/ListInterval.cpp \
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

