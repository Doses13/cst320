##############################################
# Makefile for CST320 labs
#
# Author: Philip Howard
# phil.howard@oit.edu
#
# Nov. 24, 2015
#

COPTS=-Wall -g -c  -O0
OBJS=main.o \
	 langlex.o \

all: lang

clean:
	rm -f $(OBJS)
	rm -f langlex.c
	rm -f lang
	rm -f out

.c.o:
	g++ $(COPTS) $? -o $@

.cpp.o:
	g++ $(COPTS) $? -o $@

main.o: main.cpp langlex.c 
	g++ $(COPTS) main.cpp -o main.o

langlex.c: lang.l
	flex -o langlex.c lang.l

langlex.o: langlex.c
	g++ $(COPTS) -Wno-sign-compare $? -o $@
lang: $(OBJS)
	g++ $(OBJS) -o lang

