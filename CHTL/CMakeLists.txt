add_library(CHTL
    CHTL/CHTLLexer/Lexer.cpp
)

# Make headers in the project root available to targets that link against CHTL
target_include_directories(CHTL PUBLIC
    ${CMAKE_SOURCE_DIR}
)

# Source files for the CHTL library will be added here in the future.
# For example:
# target_sources(CHTL PRIVATE
#     CHTL/CHTLParser/Parser.cpp
#     # ... other source files
# )