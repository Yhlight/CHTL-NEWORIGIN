# JS CMakeLists.txt
add_library(JS STATIC
    JSCompiler.h
    JSCompiler.cpp
)

target_include_directories(JS PUBLIC
    ${CMAKE_SOURCE_DIR}/JS
)

target_link_libraries(JS PUBLIC
    Util
    ThirdParty
)