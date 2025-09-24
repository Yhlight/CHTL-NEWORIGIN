# Add component subdirectories.
add_subdirectory(Util)
add_subdirectory(CHTL)
add_subdirectory(Test)

# Define the main executable for the CHTL compiler
add_executable(chtl main.cpp)

# Link the implemented component libraries to the main executable.
target_link_libraries(chtl
    PRIVATE
    CHTL_Util
    CHTL_Compiler
)
