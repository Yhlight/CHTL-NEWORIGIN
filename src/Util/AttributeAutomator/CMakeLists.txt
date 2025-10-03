add_library(AttributeAutomator AttributeAutomator.cpp)

target_include_directories(AttributeAutomator PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Link against CHTL because AttributeAutomator uses CHTL nodes
target_link_libraries(AttributeAutomator PRIVATE CHTL)