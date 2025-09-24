# This will be a library containing the CHTL_JS compiler components
add_library(CHTL_JS STATIC
    CHTL_JS.cpp
)

# Publicly expose the CHTL_JS directory for includes
target_include_directories(CHTL_JS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
