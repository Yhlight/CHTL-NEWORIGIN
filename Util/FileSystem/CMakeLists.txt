# Create a static library for the FileSystem component.
add_library(FileSystem_lib STATIC
    FileSystem.cpp
    FileSystem.h
)

# Make the library's headers available to other components.
target_include_directories(FileSystem_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link this library to the main Util INTERFACE library.
# Any target that links against Util will now also get FileSystem_lib.
target_link_libraries(Util INTERFACE FileSystem_lib)
