# This library will contain the miniz single-file zip library
add_library(ThirdParty_Lib STATIC
    miniz.c
)

# Add the current directory to the public include path so other targets can find miniz.h
target_include_directories(ThirdParty_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
