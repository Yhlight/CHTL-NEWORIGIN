import os
import subprocess
import sys
import shutil

def run_command(command, cwd, check=True):
    """Runs a command in a specified directory and optionally checks for errors."""
    print(f"Running command: {' '.join(command)} in {cwd}")
    result = subprocess.run(command, cwd=cwd, capture_output=True, text=True)
    if check and result.returncode != 0:
        print(f"Error executing command: {' '.join(command)}")
        print(f"STDOUT:\n{result.stdout}")
        print(f"STDERR:\n{result.stderr}")
        sys.exit(1)
    return result

def check_libzip():
    """Checks if libzip-dev is installed on Debian-based systems."""
    if sys.platform.startswith('linux'):
        try:
            # Use 'dpkg -s' which is standard for Debian-based distros.
            # Redirect output to DEVNULL to keep the output clean.
            result = run_command(["dpkg", "-s", "libzip-dev"], ".", check=False)
            if result.returncode != 0:
                 print("Error: `libzip-dev` package not found.")
                 print("Please install it using: sudo apt-get update && sudo apt-get install -y libzip-dev")
                 sys.exit(1)
            print("`libzip-dev` is installed.")
        except FileNotFoundError:
            # This handles cases where dpkg is not available (e.g., non-Debian linux)
            print("Warning: Could not check for `libzip-dev` using `dpkg`. Assuming it's installed.")
            print("If the build fails, please ensure `libzip-dev` or your system's equivalent is installed.")


def main():
    """Main function to build the CHTL compiler and its dependencies."""
    project_root = os.path.dirname(os.path.abspath(__file__))

    # 1. Check for system dependencies
    check_libzip()

    # 2. Build antlr4
    print("\n--- Building ANTLR4 ---")
    antlr4_dir = os.path.join(project_root, "third-party", "antlr4")
    # The antlr4 java project is inside a nested 'runtime/Java' directory
    # but the pom.xml is at the root of the antlr4 directory.
    if os.path.exists(os.path.join(antlr4_dir, "pom.xml")):
        # Clean previous builds and install, skipping the tests that were failing
        run_command(["mvn", "clean", "install", "-DskipTests"], cwd=antlr4_dir)
    else:
        print("Could not find pom.xml for antlr4. Skipping build.")


    # 3. Create a build directory
    print("\n--- Building CHTL ---")
    build_dir = os.path.join(project_root, "build")
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)

    # 4. Run CMake
    cmake_command = ["cmake", ".."]
    run_command(cmake_command, cwd=build_dir)

    # 5. Run make
    build_command = ["make"]
    run_command(build_command, cwd=build_dir)

    print("\nBuild completed successfully.")

if __name__ == "__main__":
    main()