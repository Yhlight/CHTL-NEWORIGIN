# 编译器调度器库

# 源文件
set(DISPATCHER_SOURCES
    CompilerDispatcher.cpp
)

# 头文件
set(DISPATCHER_HEADERS
    CompilerDispatcher.h
)

# 创建库
add_library(CompilerDispatcher STATIC ${DISPATCHER_SOURCES} ${DISPATCHER_HEADERS})

# 设置包含目录
target_include_directories(CompilerDispatcher PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(CompilerDispatcher PUBLIC cxx_std_17)