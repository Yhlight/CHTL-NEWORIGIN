# Add component subdirectories. Each of these will be built as a static library.
add_subdirectory(Util)
add_subdirectory(Scanner)
add_subdirectory(CompilerDispatcher)
add_subdirectory(CodeMerger)
add_subdirectory(CHTL)
add_subdirectory(CHTL_JS)
add_subdirectory(Test)

# Define the main executable for the CHTL compiler
add_executable(chtl main.cpp)

# Link the component libraries to the main executable.
# The libraries will be created in the subdirectories' CMakeLists.txt files.
target_link_libraries(chtl
    PRIVATE
    CHTL_Util
    CHTL_Scanner
    CHTL_CompilerDispatcher
    CHTL_CodeMerger
    CHTL_Compiler
    CHTL_JS_Compiler
)
