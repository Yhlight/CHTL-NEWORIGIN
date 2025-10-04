add_library(ImportResolver ImportResolver.cpp)

target_link_libraries(ImportResolver PRIVATE CHTL)

target_include_directories(ImportResolver PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)