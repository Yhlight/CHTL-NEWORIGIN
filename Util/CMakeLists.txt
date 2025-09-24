# Util CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

add_subdirectory(FileSystem)
add_subdirectory(StringUtil)
add_subdirectory(ZipUtil)

# Create combined Util library
add_library(Util STATIC
    FileSystem/FileSystem.cpp
    StringUtil/StringUtil.cpp
    ZipUtil/ZipUtil.cpp
)

target_include_directories(Util PUBLIC
    ${CMAKE_SOURCE_DIR}/Util
)

target_link_libraries(Util PUBLIC
    FileSystem
    StringUtil
    ZipUtil
    ThirdParty
)