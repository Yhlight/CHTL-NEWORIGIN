# Add the miniz library from its C source file.
add_library(miniz STATIC
    miniz.c
)

# Explicitly set the linker language to C for this target, since the main project is CXX.
set_target_properties(miniz PROPERTIES LINKER_LANGUAGE C)

# Publicly expose the include directory for any target linking against miniz.
# This makes miniz.h available.
target_include_directories(miniz PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
