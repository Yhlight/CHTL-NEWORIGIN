add_library(chtl-util STATIC
    ExpressionEvaluator/ExpressionEvaluator.cpp
)

target_include_directories(chtl-util PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)