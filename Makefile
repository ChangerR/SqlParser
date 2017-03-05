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

CPPS = sql.cpp scan.cpp
OBJS = allocator.o keywords.o sql.o scan.o parser.o main.o

all:main

main:$(OBJS)
	$(CXX) -o $@ $(LKFALG) $(OBJS)

clean:
	rm $(OBJS) sql.cpp sql.hpp scan.cpp *.d *.d.*

%.o:%.cpp
	$(CXX) $(REAL_CFLAGS) -o $@ $<

%.cpp:%.l
	$(LEX) -CF -o $@ $<

%.cpp:%.yy
	$(YACC) -d -o $@ $<

%.d:%.cpp
	@set -e; rm -f $@; $(CC) -MM $< $(INCLUDE) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(OBJS:.o=.d)

sql.hpp:sql.cpp
sql.o:sql.cpp
scan.o:scan.cpp
keywords.o:sql.hpp keywords.cpp