import os
import subprocess
import sys

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Running command: {' '.join(command)}")
    result = subprocess.run(command, cwd=cwd)
    if result.returncode != 0:
        print(f"Error executing command: {' '.join(command)}")
        sys.exit(1)

def main():
    """Main function to build the CHTL compiler."""
    # Get the absolute path of the script's directory (project root)
    project_root = os.path.dirname(os.path.abspath(__file__))

    # Create a build directory
    build_dir = os.path.join(project_root, "build")
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # Run CMake
    cmake_command = ["cmake", ".."]
    run_command(cmake_command, cwd=build_dir)

    # Run make (or the appropriate build command for the system)
    build_command = ["make"] # This might need to be adjusted for different build systems (e.g., "nmake" or "mingw32-make" on Windows)
    run_command(build_command, cwd=build_dir)

    print("Build completed successfully.")

if __name__ == "__main__":
    main()