# 工具库

# 源文件
set(UTIL_SOURCES
    # 这里可以添加工具类的源文件
)

# 头文件
set(UTIL_HEADERS
    # 这里可以添加工具类的头文件
)

# 创建库
add_library(Util STATIC ${UTIL_SOURCES} ${UTIL_HEADERS})

# 设置包含目录
target_include_directories(Util PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(Util PUBLIC cxx_std_17)