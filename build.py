import os
import subprocess
import sys
import platform

def run_command(command, cwd):
    """Runs a command in a specified directory and checks for errors."""
    print(f"Running command: {' '.join(command)} in {cwd}")
    try:
        subprocess.run(command, cwd=cwd, check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as e:
        print(f"Error: Command failed with exit code {e.returncode}")
        print(f"Stdout: {e.stdout}")
        print(f"Stderr: {e.stderr}")
        sys.exit(e.returncode)

def main():
    """Main function to orchestrate the build process."""
    project_root = os.path.abspath(os.path.dirname(__file__))
    build_dir = os.path.join(project_root, "build")

    # Create build directory if it doesn't exist
    if not os.path.exists(build_dir):
        print(f"Creating build directory: {build_dir}")
        os.makedirs(build_dir)

    # Configure the project with CMake
    cmake_configure_command = ["cmake", ".."]
    if platform.system() == "Windows":
        cmake_configure_command.extend(["-A", "x64"])

    run_command(cmake_configure_command, cwd=build_dir)

    # Build the project
    # The `--config Debug` is often needed for multi-config generators like Visual Studio
    cmake_build_command = ["cmake", "--build", ".", "--config", "Debug"]
    run_command(cmake_build_command, cwd=build_dir)

    print("\nBuild completed successfully!")

if __name__ == "__main__":
    main()