import os
import subprocess
import sys

def run_command(command):
    """Runs a command and prints its output."""
    try:
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, shell=True)
        for line in iter(process.stdout.readline, ''):
            sys.stdout.write(line)
        process.stdout.close()
        return_code = process.wait()
        if return_code:
            raise subprocess.CalledProcessError(return_code, command)
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {e}")
        sys.exit(1)

def main():
    """Main function to build the project."""
    # Create a build directory if it doesn't exist
    if not os.path.exists("build"):
        os.makedirs("build")

    # Change to the build directory
    os.chdir("build")

    # Run CMake and build the project
    run_command("cmake ..")
    run_command("cmake --build .")

    # Run the tests
    print("Running lexer tests...")
    run_command("./lexer_tests")
    print("Running parser tests...")
    run_command("./parser_tests")
    print("Running generator tests...")
    run_command("./generator_tests")

    # Run end-to-end test
    print("\nRunning end-to-end test with example.chtl:")
    run_command("./chtl ../example.chtl")

    # Change back to the root directory
    os.chdir("..")

if __name__ == "__main__":
    main()