# CMake generated Testfile for
# Source directory: /app/CHTL
# Build directory: /app/build/CHTL
#
# This file includes the relevant testing commands required for
# testing this directory and lists subdirectories to be tested as well.
add_test(RunComprehensiveTest "/app/build/CHTL/CHTLCompiler" "/app/main.chtl")
set_tests_properties(RunComprehensiveTest PROPERTIES  WORKING_DIRECTORY "/app" _BACKTRACE_TRIPLES "/app/CHTL/CMakeLists.txt;11;add_test;/app/CHTL/CMakeLists.txt;0;")
subdirs("CHTL")
