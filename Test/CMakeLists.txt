# Add the executable for the tests
add_executable(chth_tests main.cpp LexerTest.cpp ParserTest.cpp GeneratorTest.cpp CssGeneratorTest.cpp JsGeneratorTest.cpp TestUtil.cpp ParserBugsTest.cpp)

# Make sure the test executable can find the catch.hpp header in the root directory
target_include_directories(chth_tests PUBLIC ${CMAKE_SOURCE_DIR})

# Link the test executable against the CHTL library to access its code
target_link_libraries(chth_tests PRIVATE chtl_lib)

# Add a test to be run by CTest
add_test(NAME RunTests COMMAND chth_tests)