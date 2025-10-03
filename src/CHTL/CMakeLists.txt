add_subdirectory(ExpressionParser)

add_library(CHTL
    CHTL.cpp
    CHTLParser/CHTLParser.cpp
    CHTLLexer/CHTLLexer.cpp
    CHTLState/DefaultState.cpp
    CHTLState/TagState.cpp
    CHTLState/ScriptState.cpp
    CHTLState/StyleState.cpp
    CHTLState/CommentState.cpp
    CHTLState/TextState.cpp
    CHTLState/IfBehaviorState.cpp
    CHTLState/DeclarationState.cpp
)

target_link_libraries(CHTL PRIVATE GlobalStylesheet)

target_include_directories(CHTL PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/CHTLState"
    "${CMAKE_CURRENT_SOURCE_DIR}/CHTLNode"
    "${CMAKE_CURRENT_SOURCE_DIR}/CHTLLexer"
    "${CMAKE_CURRENT_SOURCE_DIR}/CHTLParser"
)

# Add source files from subdirectories as they are created
# target_sources(CHTL PRIVATE
#     CHTLState/CHTLState.cpp
#     ...
# )