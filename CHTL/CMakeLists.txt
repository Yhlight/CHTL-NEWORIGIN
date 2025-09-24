add_library(chtl_core STATIC
  CHTLState/State.h
  CHTLStrategy/Strategy.h
  CHTLContext/Context.h
)

target_include_directories(chtl_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(chtl_core PUBLIC cxx_std_20)

