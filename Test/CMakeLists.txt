# This directory is for tests, which will not be part of the main build unless
# a specific build flag is enabled. For now, we just set up the structure.

# Enable testing for this project
enable_testing()

# Add test subdirectories
add_subdirectory(UtilTest)
add_subdirectory(TokenTest)
add_subdirectory(AstTest)
