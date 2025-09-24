# This library will contain utility code (e.g., for file system, string manipulation)
add_library(Util_Lib STATIC
    FileSystem/File.cpp
)

# Add the current directory to the public include path for future headers
target_include_directories(Util_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
