import os
import subprocess

def run_command(command):
    """Runs a command and prints its output."""
    print(f"Running command: {' '.join(command)}")
    result = subprocess.run(command, capture_output=True, text=True)
    if result.stdout:
        print(result.stdout)
    if result.stderr:
        print(result.stderr)
    if result.returncode != 0:
        print(f"Command failed with exit code {result.returncode}")
        exit(result.returncode)

def main():
    """Main function to build the project."""
    # Create build directory if it doesn't exist
    if not os.path.exists("build"):
        os.makedirs("build")

    # Change to build directory
    os.chdir("build")

    # Run CMake
    run_command(["cmake", ".."])

    # Run make
    run_command(["make"])

    # Run ctest
    run_command(["ctest", "--output-on-failure"])

if __name__ == "__main__":
    main()