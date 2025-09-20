# 代码合并器库

# 源文件
set(MERGER_SOURCES
    CodeMerger.cpp
)

# 头文件
set(MERGER_HEADERS
    CodeMerger.h
)

# 创建库
add_library(CodeMerger STATIC ${MERGER_SOURCES} ${MERGER_HEADERS})

# 设置包含目录
target_include_directories(CodeMerger PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(CodeMerger PUBLIC cxx_std_17)