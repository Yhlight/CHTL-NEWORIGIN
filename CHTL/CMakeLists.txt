add_library(chtl-lib STATIC
    CHTLLexer/CHTLLexer.cpp
    CHTLParser/CHTLParser.cpp
)

target_include_directories(chtl-lib PUBLIC
    .
    ../SharedCore
    ../Util
)

add_executable(chtl-compiler main.cpp)
target_link_libraries(chtl-compiler PRIVATE chtl-lib)