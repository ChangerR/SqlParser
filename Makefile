SRCTREE?= $(shell pwd)
CXX:=g++
OPTIMIZATION?=-Og
WARNINGS=-Wall
DEBUG?= -g -ggdb -DDEBUG=1
LEX=flex
YACC=bison
INCLUDE=
REAL_CFLAGS= $(OPTIMIZATION) $(WARNINGS) $(DEBUG) $(INCLUDE)

.PHONY: all clean

CPPS = sql.cpp scan.cpp
OBJS = keywords.o sql.o scan.o parser.o

all:$(CPPS) $(OBJS)
	echo "HAHA"
	
clean:
	rm $(OBJS) sql.cpp sql.hpp scan.cpp

sql.o:sql.cpp

scan.o:scan.cpp

%.o:%.cpp
	$(CXX) $(REAL_CFLAGS) -o $@ $<

%.cpp:%.l
	$(LEX) -o $@ $<

%.cpp:%.yy
	$(YACC) -d -o $@ $<