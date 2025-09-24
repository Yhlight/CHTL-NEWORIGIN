# AstTest CMakeLists.txt
add_executable(AstTest
    AstTest.cpp
    NodeTest.cpp
    ParserTest.cpp
)

target_include_directories(AstTest PUBLIC
    ${CMAKE_SOURCE_DIR}/Test/AstTest
)

target_link_libraries(AstTest PRIVATE
    CHTLNode
    CHTLLexer
    Util
)