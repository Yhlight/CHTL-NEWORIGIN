cmake_minimum_required(VERSION 3.16)

# CHTL Core Library
set(CHTL_SOURCES
    src/CHTLContext.cpp
    src/CHTLNode.cpp
    src/CHTLState.cpp
    src/CHTLStrategy.cpp
    src/CHTLLexer.cpp
    src/CHTLParser.cpp
    src/CHTLGenerator.cpp
)

# Create CHTL core library
add_library(chtl_core STATIC ${CHTL_SOURCES})

# Include directories
target_include_directories(chtl_core PUBLIC include)

# Compiler definitions
target_compile_definitions(chtl_core PRIVATE CHTL_VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
target_compile_definitions(chtl_core PRIVATE CHTL_VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_compile_definitions(chtl_core PRIVATE CHTL_VERSION_PATCH=${PROJECT_VERSION_PATCH})

# Link libraries (will be added as ANTLR4 is configured)
# target_link_libraries(chtl_core antlr4-runtime)

# Install configuration
install(TARGETS chtl_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include/CHTL)