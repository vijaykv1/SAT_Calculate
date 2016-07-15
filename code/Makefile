###########################################################
## Makefile for SAT Calculation project
## Authors: Varun Vijaykumar
## Date : 12 - June - 2016
###########################################################

CC = g++

HEADERS = DavisPutnam.h fileParser.h CNF.h dataManagement.h

SOURCES = DavisPutnam.cpp main.cpp fileParser.cpp CNF.cpp

CFLAGS = -std=c++11 -o

bin = sat

$(bin) : $(SOURCES)
	$(CC) $^ $(CFLAGS) $@

clean:
	rm -vrf $(bin)
