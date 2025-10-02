# Utility Libraries

set(UTIL_SOURCES
    FileSystem/FileSystem.cpp
    StringUtil/StringUtil.cpp
    ZipUtil/ZipUtil.cpp
)

set(UTIL_HEADERS
    FileSystem/FileSystem.h
    StringUtil/StringUtil.h
    ZipUtil/ZipUtil.h
)

add_library(util_lib STATIC ${UTIL_SOURCES} ${UTIL_HEADERS})

target_include_directories(util_lib 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/third-party
)

# Link miniz for ZIP functionality
target_sources(util_lib PRIVATE
    ${CMAKE_SOURCE_DIR}/third-party/miniz.c
)
