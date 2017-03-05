SRCTREE?= $(shell pwd)
CXX:=g++
OPTIMIZATION?=-O2
WARNINGS=-Wall -Wno-deprecated-register
DEBUG?= -g -ggdb -DDEBUG=1
LEX=flex
YACC=bison
INCLUDE= -I$(SRCTREE)/build/include
REAL_CFLAGS= -std=c++11 $(OPTIMIZATION) $(WARNINGS) $(DEBUG) $(INCLUDE) -c
LKFALG= -L$(SRCTREE)/build/lib -ltcmalloc

.PHONY: all clean

OBJS = allocator.o keywords.o parser.o main.o scan.o sql.o 

all:main

main:$(OBJS)
	$(CXX) -o $@ $(LKFALG) $(OBJS)

clean:
	-rm $(OBJS) sql.cpp sql.hpp scan.cpp *.d *.d.*

%.o:%.cpp
	$(CXX) $(REAL_CFLAGS) -o $@ $<

%.cpp:%.l
	$(LEX) -CF -o $@ $<

%.cpp:%.yy
	$(YACC) -d -o $@ $<

$(SRCTREE)/build/include/gperftools/tcmalloc.h:
	cd $(SRCTREE)/3rd/gperftools && ./autogen.sh && ./configure --prefix=$(SRCTREE)/build && make install

%.d:%.cpp
	rm -f $@; $(CC) -MM $< $(INCLUDE) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

sql.hpp:sql.cpp
scan.cpp:scan.l

-include $(OBJS:.o=.d)
