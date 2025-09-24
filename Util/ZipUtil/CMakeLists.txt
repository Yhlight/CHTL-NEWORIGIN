# ZipUtil CMakeLists.txt
add_library(ZipUtil STATIC
    ZipUtil.h
    ZipUtil.cpp
)

target_include_directories(ZipUtil PUBLIC
    ${CMAKE_SOURCE_DIR}/Util/ZipUtil
)

target_link_libraries(ZipUtil PUBLIC ThirdParty)