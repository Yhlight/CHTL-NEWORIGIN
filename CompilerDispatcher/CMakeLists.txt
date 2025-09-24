# This defines the main executable for the CHTL compiler
add_executable(chtl_compiler
    main.cpp
)

# Link the executable against the project's libraries
target_link_libraries(chtl_compiler
    PRIVATE
    CHTL_Lib
    Util_Lib
    ThirdParty_Lib
    Scanner_Lib
)
