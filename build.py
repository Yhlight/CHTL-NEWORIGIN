import os
import subprocess
import sys
import argparse

def run_command(command):
    """Runs a command and prints its output in real-time."""
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1, universal_newlines=True)
    for line in process.stdout:
        print(line, end='')
    process.wait()
    return process.returncode

def main():
    """Main function to run the build process."""
    parser = argparse.ArgumentParser(description="Builds the CHTL project.")
    parser.add_argument("--no-tests", action="store_true", help="Do not run tests after building.")
    args = parser.parse_args()

    build_dir = "build"

    # Create build directory if it doesn't exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Store the original directory
    original_dir = os.getcwd()

    # Change to the build directory
    os.chdir(build_dir)

    try:
        # Configure the project with CMake
        print("--- Configuring with CMake ---")
        cmake_configure_command = ["cmake", ".."]
        if run_command(cmake_configure_command) != 0:
            print("CMake configuration failed.")
            sys.exit(1)

        # Build the project
        print("\n--- Building with CMake ---")
        cmake_build_command = ["cmake", "--build", "."]
        if run_command(cmake_build_command) != 0:
            print("CMake build failed.")
            sys.exit(1)

        print("\nBuild successful!")

        # Run tests by default
        if not args.no_tests:
            print("\n--- Running Tests ---")
            ctest_command = ["ctest", "--verbose"]
            if run_command(ctest_command) != 0:
                print("Tests failed.")
                sys.exit(1)
            print("\nAll tests passed!")

    finally:
        # Return to the original directory
        os.chdir(original_dir)

if __name__ == "__main__":
    main()