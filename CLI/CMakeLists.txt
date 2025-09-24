# CLI CMakeLists.txt
add_executable(chtl
    main.cpp
    CLIApp.h
    CLIApp.cpp
    CommandLineParser.h
    CommandLineParser.cpp
    CompilerMonitor.h
    CompilerMonitor.cpp
)

target_include_directories(chtl PUBLIC
    ${CMAKE_SOURCE_DIR}/CLI
)

target_link_libraries(chtl PRIVATE
    CHTL
    CHTL_JS
    Scanner
    CompilerDispatcher
    CodeMerger
    Util
    ThirdParty
)

# Install CLI tool
install(TARGETS chtl
        RUNTIME DESTINATION bin)