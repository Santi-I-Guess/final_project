# helper Makefile for top directory Makefile
# since no source file in a subdirectory relies on a different subfolder's
#         source files, wildcard functions work nicely

SRC_FILES = $(wildcard *.cpp)
BUILD_DIR = ../../build
OBJECTS = $(SRC_FILES:%.cpp=$(BUILD_DIR)/%.o)

CXX = g++
CXXFLAGS_DEBUG = -g
CXXFLAGS_WARN  = -Wall -Werror -Wextra -Wconversion -Wdouble-promotion \
				 -Wunreachable-code -Wshadow -Wpedantic
CPPVERSION = -std=c++17

all: $(OBJECTS)

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CPPVERSION) $(CXXFLAGS_DEBUG) $(CXXFLAGS_WARN) -o $@ -c $<

.DEFAULT: all
