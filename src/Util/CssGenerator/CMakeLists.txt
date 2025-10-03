add_library(CssGenerator CssGenerator.cpp)

target_include_directories(CssGenerator PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Link against CHTL because CssGenerator uses CHTL nodes
target_link_libraries(CssGenerator PRIVATE CHTL ExpressionEvaluator)