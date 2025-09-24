# CompilerDispatcher CMakeLists.txt
add_library(CompilerDispatcher STATIC
    Dispatcher.h
    Dispatcher.cpp
    TaskQueue.h
    TaskQueue.cpp
    ParallelExecutor.h
    ParallelExecutor.cpp
)

target_include_directories(CompilerDispatcher PUBLIC
    ${CMAKE_SOURCE_DIR}/CompilerDispatcher
)

target_link_libraries(CompilerDispatcher PUBLIC
    Scanner
    Util
)