# FileSystem CMakeLists.txt
add_library(FileSystem STATIC
    FileSystem.h
    FileSystem.cpp
)

target_include_directories(FileSystem PUBLIC
    ${CMAKE_SOURCE_DIR}/Util/FileSystem
)

target_link_libraries(FileSystem PUBLIC ThirdParty)