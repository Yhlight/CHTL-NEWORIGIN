add_library(chtl_core INTERFACE)
target_include_directories(chtl_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(chtl_core INTERFACE cxx_std_20)

