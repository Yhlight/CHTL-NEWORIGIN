# CSS编译器库

# 源文件
set(CSS_SOURCES
    CSSCompiler.cpp
)

# 头文件
set(CSS_HEADERS
    CSSCompiler.h
)

# 创建库
add_library(CSSCompiler STATIC ${CSS_SOURCES} ${CSS_HEADERS})

# 设置包含目录
target_include_directories(CSSCompiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(CSSCompiler PUBLIC cxx_std_17)