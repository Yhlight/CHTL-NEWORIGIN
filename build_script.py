import os
import subprocess
import sys

def main():
    # Ensure we are in the project root
    if not os.path.exists("CMakeLists.txt"):
        print("Error: This script must be run from the project root directory.", file=sys.stderr)
        sys.exit(1)

    # Create build directory
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Run CMake
    print("--- Running CMake ---")
    cmake_command = ["cmake", "-S", ".", "-B", build_dir]
    result = subprocess.run(cmake_command, capture_output=True, text=True)
    if result.returncode != 0:
        print("CMake failed:", file=sys.stderr)
        print(result.stdout, file=sys.stdout)
        print(result.stderr, file=sys.stderr)
        sys.exit(1)
    print(result.stdout)
    print(result.stderr)


    # Run Make
    print("\n--- Running Make ---")
    make_command = ["make", "-C", build_dir]
    result = subprocess.run(make_command, capture_output=True, text=True)
    if result.returncode != 0:
        print("Make failed:", file=sys.stderr)
        print(result.stdout, file=sys.stdout)
        print(result.stderr, file=sys.stderr)
        sys.exit(1)
    print(result.stdout)
    print(result.stderr)


    # Run tests
    print("\n--- Running Tests ---")
    test_executable = os.path.join(build_dir, "Test", "run-tests")
    test_command = [test_executable]
    result = subprocess.run(test_command, capture_output=True, text=True)
    if result.returncode != 0:
        print("Tests failed:", file=sys.stderr)
        print(result.stdout, file=sys.stdout)
        print(result.stderr, file=sys.stderr)
        sys.exit(1)

    print(result.stdout)
    print("All tests passed!")


if __name__ == "__main__":
    main()