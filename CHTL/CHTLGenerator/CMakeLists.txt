add_library(CHTLGenerator STATIC
    Generator.cpp
)

# The generator needs to know about the Node definitions.
target_link_libraries(CHTLGenerator PUBLIC
    CHTLNode
)

# Expose the generator's own headers.
target_include_directories(CHTLGenerator PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
