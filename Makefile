SRCTREE?= $(shell pwd)
CC?=gcc
CXX?=g++
OPTIMIZATION?=-O2
WARNINGS=-Wall -Wno-deprecated-register
DEBUG?= -g -ggdb -DDEBUG=1 -DYYDEBUG=1
LEX=flex
YACC=bison
INCLUDE= -Iinclude -Ibuild/include
REAL_CFLAGS= -std=gnu++11 -fPIC $(OPTIMIZATION) $(WARNINGS) $(DEBUG) $(INCLUDE) -c
LKFALG= -Lbuild/lib -L. -Wl,-rpath=$(SRCTREE)/build/lib -Wl,-rpath=$(SRCTREE)
LDLIB = -ltcmalloc
OS = $(shell uname -s)
SONAME?=so
ifeq ("$(OS)","Darwin")
	SONAME:=dylib
	LKFALG=-Lbuild/lib -L. -Wl,-F$(SRCTREE)/build/lib -Wl,-F$(SRCTREE)
endif
ifeq ("MINGW","$(findstring MINGW,$(OS))")
	SONAME:=dll
	INCLUDE+= -I3rd/gperftools/src/windows 
	LKFALG+= -L3rd/gperftools/x64/Debug
	LDLIB=-ltcmalloc_minimal
endif

.PHONY: all clean test

OBJS = allocator.o keywords.o parser.o scan.o sql.o 
TESTOBJS = test/test.o 

all:libsqlparser.$(SONAME) testparser

libsqlparser.$(SONAME):$(OBJS)
	$(CXX) -shared $(LKFALG) $(OBJS) $(LDLIB) -o $@

testparser:$(OBJS) $(TESTOBJS)
	$(CXX) $(LKFALG) $(TESTOBJS) $(LDLIB) $(OBJS) -o $@

clean:
	-rm -f $(OBJS) sql.cpp include/sql.hpp scan.cpp *.d sql.output libsqlparser.$(SONAME) $(TESTOBJS) testparser 2>/dev/null

distclean:clean
	@if [ -d build ] ; then \
		cd 3rd/gperftools; \
		make distclean; \
		cd $(SRCTREE); \
		rm -rf build ; \
	fi 

test:
	./testparser test/test.sql

%.o:%.cpp
	$(CXX) $(REAL_CFLAGS) -o $@ $<

%.cpp:%.l
	$(LEX) -CF -o $@ $<

%.cpp:%.yy
	$(YACC) --debug --report=itemset --defines=include/$*.hpp -o $@ $<

build/include/gperftools/tcmalloc.h:
	cd 3rd/gperftools && ./autogen.sh && ./configure --prefix=$(SRCTREE)/build && make install

include/kwlist.h:sql.yy
	python keyword.py $< $@

%.d:%.cpp 
	rm -f $@; $(CC) -MM $< $(INCLUDE) > $@.$$$$; \
	if [ -s $@.$$$$ ] ; then \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	fi; \
	rm -f $@.$$$$; 

	
allocator.d:build/include/gperftools/tcmalloc.h

include/sql.hpp:sql.cpp
scan.cpp:scan.l

-include $(OBJS:.o=.d)
