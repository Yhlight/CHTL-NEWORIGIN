add_library(ExpressionParser ExpressionParser.cpp)

target_include_directories(ExpressionParser PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Link against CHTL because ExpressionParser uses CHTL nodes and the main parser
target_link_libraries(ExpressionParser PRIVATE CHTL)