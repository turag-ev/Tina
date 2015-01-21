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
SRCFILES = \
	$(wildcard *.cpp) \
	$(wildcard tina/*.c) \
	$(wildcard helper/*.cpp) \
	../tina++/utils/cost.cpp \
	../tina++/statemachine/state.cpp
  
# make everything public :P
CXXFLAGS += -Dprivate=public -Dprotected=public

# no config
CXXFLAGS += -DTURAG_NO_PROJECT_CONFIG -DTURAG_NO_PLATFORM_CONFIG
CFLAGS += -DTURAG_NO_PROJECT_CONFIG -DTURAG_NO_PLATFORM_CONFIG
  
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

test: $(PROGRAM)
	@bash -c "echo -e '\e[32m--- RUN TESTS ---\e[0m'"
	@./$(PROGRAM)
	
-include $(wildcard *.o.d)
