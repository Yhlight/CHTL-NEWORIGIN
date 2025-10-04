add_library(TemplateEvaluator STATIC
    TemplateEvaluator.cpp
)

target_include_directories(TemplateEvaluator PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(TemplateEvaluator PRIVATE CHTL)