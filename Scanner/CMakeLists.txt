# 统一扫描器库

# 源文件
set(SCANNER_SOURCES
    UnifiedScanner.cpp
)

# 头文件
set(SCANNER_HEADERS
    UnifiedScanner.h
)

# 创建库
add_library(Scanner STATIC ${SCANNER_SOURCES} ${SCANNER_HEADERS})

# 设置包含目录
target_include_directories(Scanner PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(Scanner PUBLIC cxx_std_17)