# StringUtil CMakeLists.txt
add_library(StringUtil STATIC
    StringUtil.h
    StringUtil.cpp
)

target_include_directories(StringUtil PUBLIC
    ${CMAKE_SOURCE_DIR}/Util/StringUtil
)