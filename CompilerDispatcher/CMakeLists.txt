add_library(dispatcher STATIC
  Dispatcher.h
  Dispatcher.cpp
)

target_include_directories(dispatcher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Scanner)
target_link_libraries(dispatcher PUBLIC scanner chtl_core chtljs_core)
target_compile_features(dispatcher PUBLIC cxx_std_20)

