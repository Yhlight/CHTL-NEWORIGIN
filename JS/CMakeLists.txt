# JavaScript编译器库

# 源文件
set(JS_SOURCES
    JSCompiler.cpp
)

# 头文件
set(JS_HEADERS
    JSCompiler.h
)

# 创建库
add_library(JSCompiler STATIC ${JS_SOURCES} ${JS_HEADERS})

# 设置包含目录
target_include_directories(JSCompiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(JSCompiler PUBLIC cxx_std_17)