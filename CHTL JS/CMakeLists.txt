# CHTL JS核心库

# 源文件
set(CHTLJS_SOURCES
    CHTLJSContext.cpp
    CHTLJSLexer/CHTLJSLexer.cpp
    CHTLJSParser/CHTLJSParser.cpp
    CHTLJSGenerator/CHTLJSGenerator.cpp
    CHTLJSNode/CHTLJSNode.cpp
)

# 头文件
set(CHTLJS_HEADERS
    CHTLJSContext.h
    CHTLJSLexer/CHTLJSLexer.h
    CHTLJSParser/CHTLJSParser.h
    CHTLJSGenerator/CHTLJSGenerator.h
    CHTLJSNode/CHTLJSNode.h
)

# 创建库
add_library(CHTJSCore STATIC ${CHTLJS_SOURCES} ${CHTLJS_HEADERS})

# 设置包含目录
target_include_directories(CHTJSCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置编译选项
target_compile_features(CHTJSCore PUBLIC cxx_std_17)