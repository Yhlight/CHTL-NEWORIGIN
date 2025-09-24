# Create a static library for the StringUtil component.
add_library(StringUtil_lib STATIC
    StringUtil.cpp
    StringUtil.h
)

# Make the library's headers available to other components.
target_include_directories(StringUtil_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link this library to the main Util INTERFACE library.
# Any target that links against Util will now also get StringUtil_lib.
target_link_libraries(Util INTERFACE StringUtil_lib)
