add_library(GlobalStylesheet GlobalStylesheet.cpp)

target_include_directories(GlobalStylesheet PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)