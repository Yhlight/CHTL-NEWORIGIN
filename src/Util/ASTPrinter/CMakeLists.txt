add_library(ASTPrinter ASTPrinter.cpp)

target_include_directories(ASTPrinter PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Link against CHTL because ASTPrinter uses CHTL nodes
target_link_libraries(ASTPrinter PRIVATE CHTL)