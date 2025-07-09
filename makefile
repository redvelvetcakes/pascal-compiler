# Makefile for the tips compiler

# variables used in the following rules
LEX      = flex
CXX      = g++
CC       = gcc
RM       = rm
# -g generate debug information for gdb
# -Wno-c++11-extensions silence the c++11 error warnings
# -std=c++11 assert that we are using c++11
CXXFLAGS = -g
CXXFLAGS = -g -Wno-c++11-extensions
CXXFLAGS = -g -std=c++11
CCFLAGS  = -g
# don't ever remove these file types
.PRECIOUS = *.l *.h *.cpp [Mm]akefile

tips: lex.yy.o driver.o parser.o nodes.o
	$(CXX) $(CXXFLAGS) -o tips lex.yy.o driver.o parser.o nodes.o

#     -o flag specifies the output file
#
#     The above rule could be written with macros as
#        $(CXX) $(CXXFLAGS) -o $@ $^

driver.o: driver.cpp parser.h nodes.h lexer.h
	$(CXX) $(CXXFLAGS) -o driver.o -c driver.cpp
#      -c flag specifies stop after compiling, do not link

parser.o: parser.cpp parser.h lexer.h nodes.h
	$(CXX) $(CXXFLAGS) -o parser.o -c parser.cpp

nodes.o: nodes.cpp nodes.h lexer.h
	$(CXX) $(CXXFLAGS) -o nodes.o -c nodes.cpp

lex.yy.o: lex.yy.c lexer.h
	$(CC) $(CCFLAGS) -o lex.yy.o -c lex.yy.c

lex.yy.c: rules.l lexer.h
	$(LEX) -o lex.yy.c rules.l

clean: 
	$(RM) *.o lex.yy.c tips
#   delete all generated files	

ring:
	ls $(.PRECIOUS)
#   tell us what is precious here
