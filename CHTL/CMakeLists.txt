# Order matters here due to dependencies.
add_subdirectory(Common)
add_subdirectory(CHTLContext)
add_subdirectory(CHTLLexer)
add_subdirectory(CHTLNode)
add_subdirectory(CHTLParser)
add_subdirectory(CHTLGenerator)

# Define CHTL as an INTERFACE library to group all components.
add_library(CHTL INTERFACE)

# The main executable needs the top-level CHTL directory to find component headers
# like "CHTLLexer/Lexer.h"
target_include_directories(CHTL INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Link the CHTL library to all its components.
target_link_libraries(CHTL INTERFACE
    CHTLCommon
    CHTLContext
    CHTLLexer
    CHTLNode
    CHTLParser
    CHTLGenerator
)
