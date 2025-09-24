# Define the Util library
add_library(Util STATIC
    StringUtils.cpp
)

# Public headers for the Util library
target_include_directories(Util
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
