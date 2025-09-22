# Since this directory only contains headers for the AST nodes,
# we create an INTERFACE library. This doesn't produce a compiled
# artifact but can have properties like include directories.
add_library(CHTLNode INTERFACE)

# Add the current directory to the public include directories of the target.
# This makes the node headers available to any other target that links
# against CHTLNode.
target_include_directories(CHTLNode INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
