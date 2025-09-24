# This library will contain the Unified Scanner and related components
add_library(Scanner_Lib STATIC
    UnifiedScanner.cpp
)

target_include_directories(Scanner_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
