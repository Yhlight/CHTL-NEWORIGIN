add_library(scanner STATIC
  UnifiedScanner.cpp
)

target_include_directories(scanner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(scanner PUBLIC cxx_std_20)

