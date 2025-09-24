# Defines the CHTL_Util library, explicitly listing all its source and header files.
# This is a more robust and modern CMake practice.
add_library(CHTL_Util STATIC
    StringUtil/StringUtil.h
    StringUtil/StringUtil.cpp
    FileSystem/FileSystemUtil.h
    FileSystem/FileSystemUtil.cpp
    ZipUtil/ZipUtil.h
    ZipUtil/ZipUtil.cpp
    ${CMAKE_SOURCE_DIR}/miniz.c
)

# Exposes include directories to consumers of this library.
target_include_directories(CHTL_Util
    PUBLIC
    # Add the parent 'src' directory so includes can be #include "Util/..."
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    # Add the project root so miniz.h can be found
    ${CMAKE_SOURCE_DIR}
)

# miniz.c needs this define to build its implementation
target_compile_definitions(CHTL_Util PRIVATE MINIZ_IMPL)
