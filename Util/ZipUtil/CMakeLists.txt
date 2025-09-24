# Create a static library for the ZipUtil component.
add_library(ZipUtil_lib STATIC
    ZipUtil.cpp
    ZipUtil.h
)

# Make the library's headers available.
target_include_directories(ZipUtil_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link this library to the main Util INTERFACE library.
target_link_libraries(Util INTERFACE ZipUtil_lib)

# ZipUtil needs to link against the miniz library to function.
# We make this PUBLIC so that any consumer of ZipUtil_lib also links to miniz.
target_link_libraries(ZipUtil_lib PUBLIC miniz)
