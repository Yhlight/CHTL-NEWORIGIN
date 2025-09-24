# CHTLNode CMakeLists.txt
add_library(CHTLNode STATIC
    BaseNode.h
    BaseNode.cpp
    ElementNode.h
    ElementNode.cpp
    TextNode.h
    TextNode.cpp
    CommentNode.h
    CommentNode.cpp
    TemplateNode.h
    TemplateNode.cpp
    CustomNode.h
    CustomNode.cpp
    StyleNode.h
    StyleNode.cpp
    ScriptNode.h
    ScriptNode.cpp
    OriginNode.h
    OriginNode.cpp
    ImportNode.h
    ImportNode.cpp
    ConfigNode.h
    ConfigNode.cpp
    NamespaceNode.h
    NamespaceNode.cpp
    OperatorNode.h
    OperatorNode.cpp
    BehaviorNode.h
    BehaviorNode.cpp
    ConditionalNode.h
    ConditionalNode.cpp
)

target_include_directories(CHTLNode PUBLIC
    ${CMAKE_SOURCE_DIR}/CHTL/CHTLNode
)

target_link_libraries(CHTLNode PUBLIC
    CHTLLexer
    Util
)