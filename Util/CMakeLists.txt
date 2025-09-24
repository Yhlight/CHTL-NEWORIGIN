# Create an INTERFACE library to group all utility sub-libraries.
# This allows other targets to link against "Util" and get all sub-libraries transitively.
add_library(Util INTERFACE)

# Add subdirectories which will define their own libraries and link them to the Util interface library.
add_subdirectory(StringUtil)
add_subdirectory(FileSystem)
add_subdirectory(ZipUtil)

# Publicly expose the Util directory for includes.
# Any target linking to Util will automatically get this include path.
target_include_directories(Util INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
