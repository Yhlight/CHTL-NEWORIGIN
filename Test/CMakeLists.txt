add_executable(run_tests
    TestMain.cpp
    LexerTest.cpp
    ParserTest.cpp
    GeneratorTest.cpp
    LoaderTest.cpp
    ConditionalRenderingTest.cpp
    ConstraintTest.cpp
    ConfigurationTest.cpp
    SpecializationParsingTest.cpp
    SpecializationGeneratorTest.cpp
    CHTLJSTest.cpp
    ImportTest.cpp
)

target_link_libraries(run_tests PRIVATE CHTL CHTLJS)

# Since catch.hpp is a header-only library, we just need to make sure
# the compiler can find it.
target_include_directories(run_tests PRIVATE ../)