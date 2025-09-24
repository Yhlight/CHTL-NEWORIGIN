# Scanner CMakeLists.txt
add_library(Scanner STATIC
    StateMachine.h
    StateMachine.cpp
    Strategy.h
    Strategy.cpp
    UnifiedScanner.h
    UnifiedScanner.cpp
    PlaceholderStrategy.h
    PlaceholderStrategy.cpp
    SliceStrategy.h
    SliceStrategy.cpp
)

target_include_directories(Scanner PUBLIC
    ${CMAKE_SOURCE_DIR}/Scanner
)

target_link_libraries(Scanner PUBLIC
    Util
    ThirdParty
)