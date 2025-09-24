# UtilTest CMakeLists.txt
add_executable(UtilTest
    UtilTest.cpp
)

target_include_directories(UtilTest PUBLIC
    ${CMAKE_SOURCE_DIR}/Test/UtilTest
)

target_link_libraries(UtilTest PRIVATE
    Util
    ThirdParty
)