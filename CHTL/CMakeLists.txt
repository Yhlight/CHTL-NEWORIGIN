# CHTL CMakeLists.txt
add_subdirectory(CHTLLexer)
add_subdirectory(CHTLNode)
add_subdirectory(CHTLParser)
add_subdirectory(CHTLGenerator)
add_subdirectory(CHTLState)
add_subdirectory(CHTLStrategy)
add_subdirectory(CHTLContext)
add_subdirectory(CHTLLoader)
add_subdirectory(CHTLManage)
add_subdirectory(CHTLIOStream)
add_subdirectory(CMODSystem)

# Create combined CHTL library
add_library(CHTL STATIC
    # Main CHTL interface
)

target_include_directories(CHTL PUBLIC
    ${CMAKE_SOURCE_DIR}/CHTL
)

target_link_libraries(CHTL PUBLIC
    CHTLLexer
    CHTLNode
    CHTLParser
    CHTLGenerator
    CHTLState
    CHTLStrategy
    CHTLContext
    CHTLLoader
    CHTLManage
    CHTLIOStream
    CMODSystem
    Scanner
    Util
    ThirdParty
)