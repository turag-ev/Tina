# Programs
CC = gcc
CXX = g++
RM = rm

# Compiler flags
CFLAGS = -Wall -I.. -I. -g -O0
CXXFLAGS = $(CFLAGS) -std=c++0x
LDFLAGS = -Wall -std=c++0x -g -O0

# Output file
PROGRAM = tests

# Input files
SRCFILES = $(wildcard *.cpp) ../tina++/utils/cost.cpp
LIBS = -lboost_unit_test_framework

################################################################################

OBJECTS1 = $(SRCFILES)
OBJECTS2 = $(OBJECTS1:%.c=%.o)
OBJECTS3 = $(OBJECTS2:%.cc=%.o)
OBJECTS4 = $(OBJECTS3:%.cpp=%.o)
OBJECTS  = $(OBJECTS4:%.cxx=%.o)

all: $(PROGRAM)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -MT $@ -MF $@.d -c $< -o $@
	
%.o: %.cc
	$(CXX) $(CXXFLAGS) -MMD -MP -MT $@ -MF $@.d -c $< -o $@
	
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -MT $@ -MF $@.d -c $< -o $@
	
%.o: %.cxx
	$(CXX) $(CXXFLAGS) -MMD -MP -MT $@ -MF $@.d -c $< -o $@

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $(PROGRAM) $(LDFLAGS) $(OBJECTS) $(LIBS)

clean:
	$(RM) -rf *.o *.o.d $(PROGRAM)
	
-include $(wildcard *.o.d)
