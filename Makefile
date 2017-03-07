SRCTREE?= $(shell pwd)
CC?=gcc
CXX?=g++
OPTIMIZATION?=-O2
WARNINGS=-Wall -Wno-deprecated-register
DEBUG?= -g -ggdb -DDEBUG=1 -DYYDEBUG=1
LEX=flex
YACC=bison
INCLUDE= -Iinclude -I$(SRCTREE)/build/include
REAL_CFLAGS= -std=c++11 -fPIC $(OPTIMIZATION) $(WARNINGS) $(DEBUG) $(INCLUDE) -c
LKFALG= -L$(SRCTREE)/build/lib -L$(SRCTREE)
LDLIB = -ltcmalloc
OS = $(shell uname -s)
SONAME?=so
ifeq ("$(OS)","Darwin")
	SONAME:=dylib
endif

.PHONY: all clean

OBJS = allocator.o keywords.o parser.o scan.o sql.o 
TESTOBJS = $(SRCTREE)/test/test.o 

all:libsqlparser.$(SONAME) testparser

libsqlparser.$(SONAME):$(OBJS)
	$(CXX) -shared $(LKFALG) $(OBJS) $(LDLIB) -o $@

testparser:libsqlparser.$(SONAME) $(TESTOBJS)
	$(CXX) $(LKFALG) $(TESTOBJS) $(LDLIB) -lsqlparser -o $@

clean:
	-rm $(OBJS) sql.cpp $(SRCTREE)/include/sql.hpp scan.cpp *.d sql.output libsqlparser.$(SONAME) $(TESTOBJS) testparser 2>/dev/null

%.o:%.cpp
	$(CXX) $(REAL_CFLAGS) -o $@ $<

%.cpp:%.l
	$(LEX) -CF -o $@ $<

%.cpp:%.yy
	$(YACC) --debug --report=itemset --defines=$(SRCTREE)/include/$*.hpp -o $@ $<

$(SRCTREE)/build/include/gperftools/tcmalloc.h:
	cd $(SRCTREE)/3rd/gperftools && ./autogen.sh && ./configure --prefix=$(SRCTREE)/build && make install

%.d:%.cpp 
	rm -f $@; $(CC) -MM $< $(INCLUDE) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

allocator.d:$(SRCTREE)/build/include/gperftools/tcmalloc.h
$(SRCTREE)/include/sql.hpp:sql.cpp
scan.cpp:scan.l

-include $(OBJS:.o=.d)
