# Makefile for CHTL Compiler

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Isrc
LDFLAGS =

# Source files
SRCS = $(wildcard src/CHTL/CHTLParser/*.cpp) \
       $(wildcard src/CHTL/CHTLLexer/*.cpp) \
       $(wildcard src/CHTL/CHTLNode/*.cpp) \
       $(wildcard src/CHTL/CHTLGenerator/*.cpp) \
       $(wildcard src/Util/FileSystem/*.cpp) \
       $(wildcard src/Util/StringUtil/*.cpp) \
       src/CHTL/main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Target executable
TARGET = chtl_compiler

# Default rule
all: $(TARGET)

# Rule to link the executable
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
