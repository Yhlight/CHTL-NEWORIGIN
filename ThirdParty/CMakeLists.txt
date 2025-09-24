# ThirdParty CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# Create ThirdParty library
add_library(ThirdParty STATIC
    miniz.c
)

# Explicitly set C language
set_target_properties(ThirdParty PROPERTIES LINKER_LANGUAGE C)

target_include_directories(ThirdParty PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/ThirdParty
)

# Compiler-specific options for miniz
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(ThirdParty PRIVATE -Wno-unused-parameter)
endif()

# ANTLR support (optional)
if(CHTL_WITH_ANTLR)
    find_package(antlr4-runtime REQUIRED)
    target_link_libraries(ThirdParty PUBLIC antlr4-runtime)
endif()