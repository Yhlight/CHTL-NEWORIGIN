add_executable(CHTL_Final
    main.cpp
)

# Link against all the project libraries to ensure the dependency chain is correct
target_link_libraries(CHTL_Final PRIVATE
    Util
    CHTL
    CHTL_JS
    Scanner
    CodeMerger
    miniz
)
