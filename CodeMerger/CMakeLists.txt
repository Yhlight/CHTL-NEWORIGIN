add_library(codemerger STATIC
  CodeMerger.h
  CodeMerger.cpp
)

target_include_directories(codemerger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(codemerger PUBLIC cxx_std_20)

