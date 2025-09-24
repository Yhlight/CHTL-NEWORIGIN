add_library(chtljs_core STATIC
  CHTLJSState.h
  CHTLJSStrategy.h
  CHTLJSContext.h
)

target_include_directories(chtljs_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(chtljs_core PUBLIC cxx_std_20)

