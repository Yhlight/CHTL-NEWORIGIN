# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -I./ -Wall -Wextra -pedantic

# Project directories
PROJECT_DIR = CHTL_Project
SRC_DIR = $(PROJECT_DIR)
TARGET = $(PROJECT_DIR)/chtl_compiler

# Find all .cpp files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp) \
          $(wildcard $(SRC_DIR)/CHTL/CHTLLexer/*.cpp) \
          $(wildcard $(SRC_DIR)/CHTL/CHTLParser/*.cpp) \
          $(wildcard $(SRC_DIR)/CHTL/CHTLGenerator/*.cpp)

# Generate object file names
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link the program
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets
.PHONY: all clean
