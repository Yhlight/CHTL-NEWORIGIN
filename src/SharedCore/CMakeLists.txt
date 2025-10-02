# SharedCore - Salt Bridge between CHTL and CHTL JS

set(SHARED_SOURCES
    SaltBridge.cpp
    SharedContext.cpp
)

set(SHARED_HEADERS
    SaltBridge.h
    SharedContext.h
)

add_library(shared_core STATIC ${SHARED_SOURCES} ${SHARED_HEADERS})

target_include_directories(shared_core 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/src
)
