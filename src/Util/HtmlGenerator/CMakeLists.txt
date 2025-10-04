add_library(HtmlGenerator STATIC
    HtmlGenerator.cpp
)

target_include_directories(HtmlGenerator PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(HtmlGenerator PRIVATE CHTL)