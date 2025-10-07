import subprocess
import argparse
import os
import sys

def run_command(command, cwd):
    """Runs a command in a specified directory."""
    print(f"Running command: {' '.join(command)} in {cwd}")
    process = subprocess.Popen(command, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1)
    for line in iter(process.stdout.readline, ''):
        print(line, end='')
    process.stdout.close()
    return_code = process.wait()
    if return_code:
        raise subprocess.CalledProcessError(return_code, command)

def main():
    parser = argparse.ArgumentParser(description="CHTL Build Script")
    parser.add_argument("action", choices=["configure", "build", "test", "all"], help="The build action to perform.")
    args = parser.parse_args()

    build_dir = "build"

    # The source directory is the directory where this script is located.
    source_dir = os.path.dirname(os.path.abspath(__file__))
    build_path = os.path.join(source_dir, build_dir)

    os.makedirs(build_path, exist_ok=True)

    if args.action in ["configure", "all"]:
        print("--- Configuring ---")
        try:
            cmake_configure_command = ["cmake", "-S", source_dir, "-B", build_path]
            run_command(cmake_configure_command, source_dir)
        except (subprocess.CalledProcessError, FileNotFoundError) as e:
            print(f"Error during configuration: {e}", file=sys.stderr)
            sys.exit(1)

    if args.action in ["build", "all"]:
        print("\n--- Building ---")
        try:
            cmake_build_command = ["cmake", "--build", build_path]
            run_command(cmake_build_command, source_dir)
        except (subprocess.CalledProcessError, FileNotFoundError) as e:
            print(f"Error during build: {e}", file=sys.stderr)
            sys.exit(1)

    if args.action in ["test", "all"]:
        print("\n--- Testing ---")
        try:
            # It's better to run ctest from the build directory
            ctest_command = ["ctest", "--output-on-failure"]
            run_command(ctest_command, build_path)
        except (subprocess.CalledProcessError, FileNotFoundError) as e:
            print(f"Error during testing: {e}", file=sys.stderr)
            sys.exit(1)

if __name__ == "__main__":
    main()