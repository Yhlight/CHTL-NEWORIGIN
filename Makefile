# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I.

# Source files
SRCS = CHTL/CHTLLexer/Lexer.cpp \
       CHTL/CHTLParser/Parser.cpp \
       CHTL/CHTLGenerator/Generator.cpp \
       Test/main.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Target executable
TARGET = test_runner

# Default target
all: $(TARGET)

# Linking the target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compiling source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Target to run the tests
run: all
	./$(TARGET)

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean
