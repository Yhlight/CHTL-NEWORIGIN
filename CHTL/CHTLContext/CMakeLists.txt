add_library(CHTLContext STATIC
    Context.cpp
)

# The context now only needs the shared TokenTypes enum.
target_link_libraries(CHTLContext PUBLIC
    CHTLCommon
)

# Expose the context's own headers
target_include_directories(CHTLContext PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
