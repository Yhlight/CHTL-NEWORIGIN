# TokenTest CMakeLists.txt
add_executable(TokenTest
    TokenTest.cpp
)

target_include_directories(TokenTest PUBLIC
    ${CMAKE_SOURCE_DIR}/Test/TokenTest
)

target_link_libraries(TokenTest PRIVATE
    CHTLLexer
    Util
)