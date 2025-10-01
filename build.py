import os
import subprocess
import sys

def run_command(command):
    """Runs a command and prints its output."""
    print(f"Executing: {' '.join(command)}")
    process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    for line in iter(process.stdout.readline, ''):
        sys.stdout.write(line)
    process.wait()
    return process.returncode

def main():
    """Main function to build the project."""
    # Ensure we are in the project's root directory
    if not os.path.exists("CMakeLists.txt"):
        print("Error: This script must be run from the root directory of the project.")
        sys.exit(1)

    # Create a build directory if it doesn't exist
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Change to the build directory
    os.chdir(build_dir)

    # Run CMake and build the project
    cmake_command = ["cmake", ".."]
    build_command = ["cmake", "--build", "."]

    if run_command(cmake_command) != 0:
        print("CMake configuration failed.")
        sys.exit(1)

    if run_command(build_command) != 0:
        print("Build failed.")
        sys.exit(1)

    print("Build successful!")

    # Go back to the root directory
    os.chdir("..")

    # Run tests
    print("\nRunning tests...")
    test_executable = os.path.join(build_dir, "Test", "run_tests")
    if os.path.exists(test_executable):
        if run_command([test_executable]) != 0:
            print("Tests failed.")
            sys.exit(1)
        print("Tests passed!")
    else:
        print("Test executable not found, skipping tests.")


if __name__ == "__main__":
    main()