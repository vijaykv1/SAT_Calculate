# Makefile for the project related to SAT equivalence Check algorithms.
# AUTHOR : VARUN VIJAYKUMAR
# Post fast fix build systems -- > generate a binary for the applications at hand

CC := g++ 
CFLAGS = -C -Wall 
LDFLAGS = 
SOURCES = sat.cpp
OBJECTS = $(SOURCES :.cpp=.o)
EXECUTABLE = SAT_Calculate

all: $(SOURCES) $(EXECUTABLE)
		
$(EXECUTABLE): $(SOURCES)
	$(CC) $(LDFLAGS) $(SOURCES)  -o $(EXECUTABLE) 
	echo "code built ! binary generated is " $(EXECUTABLE)

clean:
	rm -vrf $(EXECUTABLE)	