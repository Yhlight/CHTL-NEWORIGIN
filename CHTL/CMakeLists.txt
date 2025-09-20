# CHTL核心库

# 源文件
set(CHTL_SOURCES
    CHTLContext/CHTLContext.cpp
    CHTLLexer/CHTLLexer.cpp
    CHTLParser/CHTLParser.cpp
    CHTLGenerator/CHTLGenerator.cpp
    CHTLNode/CHTLNode.cpp
)

# 头文件
set(CHTL_HEADERS
    CHTLContext/CHTLContext.h
    CHTLLexer/CHTLLexer.h
    CHTLParser/CHTLParser.h
    CHTLGenerator/CHTLGenerator.h
    CHTLNode/CHTLNode.h
)

# 创建库
add_library(CHTLCore STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})

# 设置包含目录
target_include_directories(CHTLCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(CHTLCore PUBLIC cxx_std_17)