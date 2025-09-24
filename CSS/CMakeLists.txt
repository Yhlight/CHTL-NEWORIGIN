# CSS CMakeLists.txt
add_library(CSS STATIC
    CSSCompiler.h
    CSSCompiler.cpp
    ANTLRAdapter.h
    ANTLRAdapter.cpp
)

target_include_directories(CSS PUBLIC
    ${CMAKE_SOURCE_DIR}/CSS
)

target_link_libraries(CSS PUBLIC
    Util
    ThirdParty
)