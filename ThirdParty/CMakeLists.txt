add_library(miniz STATIC
  ${CMAKE_SOURCE_DIR}/miniz.c
)

target_include_directories(miniz PUBLIC ${CMAKE_SOURCE_DIR})
