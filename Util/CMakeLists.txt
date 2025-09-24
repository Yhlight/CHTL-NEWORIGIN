add_library(chtl_util STATIC
  src/StringUtil.cpp
  src/FileSystem.cpp
)

target_include_directories(chtl_util PUBLIC include)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(chtl_util PRIVATE -fno-exceptions)
endif()
