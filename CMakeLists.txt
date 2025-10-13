cmake_minimum_required(VERSION 3.20)
project(chtl LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CHTL_BUILD_TESTS "Build tests" ON)
option(CHTL_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

# Targets
add_library(chtl
    src/Lexer.cpp
    src/Parser.cpp
    src/Generator.cpp
    src/Compiler.cpp
)

target_include_directories(chtl
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/third-party
)

if (MSVC)
    target_compile_options(chtl PRIVATE /W4)
else()
    target_compile_options(chtl PRIVATE -Wall -Wextra -Wpedantic)
    if (CHTL_WARNINGS_AS_ERRORS)
        target_compile_options(chtl PRIVATE -Werror)
    endif()
endif()

add_executable(chtlc app/main.cpp)

target_link_libraries(chtlc PRIVATE chtl)

target_include_directories(chtlc PRIVATE ${PROJECT_SOURCE_DIR}/include)

include(GNUInstallDirs)
install(TARGETS chtl chtlc
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if (CHTL_BUILD_TESTS)
    enable_testing()
    add_executable(chtl_tests
        tests/test_main.cpp
        tests/test_lexer.cpp
        tests/test_parser.cpp
        tests/test_generator.cpp
    )
    target_link_libraries(chtl_tests PRIVATE chtl)
    target_include_directories(chtl_tests PRIVATE ${PROJECT_SOURCE_DIR}/third-party ${PROJECT_SOURCE_DIR}/include)
    add_test(NAME chtl_tests COMMAND chtl_tests)
endif()