# Gather all library source files
file(GLOB_RECURSE LIB_SOURCES
    "CHTLLexer/*.cpp"
    "CHTLParser/*.cpp"
    "CHTLNode/*.cpp"
    "CHTLState/*.cpp"
    "CHTLContext/*.cpp"
    "CHTLStrategy/*.cpp"
)

# Create a static library
add_library(chtl_lib STATIC ${LIB_SOURCES})

# Make the library's include directories public so other targets can find them
target_include_directories(chtl_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Create the main executable
add_executable(chtl main.cpp)

# Link the executable against the library
target_link_libraries(chtl PRIVATE chtl_lib)