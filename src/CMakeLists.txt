add_subdirectory(CHTL)

add_executable(chtl-compiler main.cpp)
target_link_libraries(chtl-compiler PRIVATE CHTL)