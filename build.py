import os
import subprocess
import shutil

# Define the build directory
build_dir = "build"

# Clean and create build directory
if os.path.exists(build_dir):
    shutil.rmtree(build_dir)
os.makedirs(build_dir)

# Run cmake and make
try:
    subprocess.run(["cmake", "-S", ".", "-B", build_dir], check=True)
    subprocess.run(["cmake", "--build", build_dir], check=True)
    print("Build successful.")
except subprocess.CalledProcessError as e:
    print(f"Build failed: {e}")
    exit(1)

# --- Run the tests directly ---
print("\n--- Running Tests Directly ---")
test_executable_path = os.path.join(build_dir, "Test", "runTests")

if os.path.exists(test_executable_path):
    try:
        # Run the test executable and capture its output
        subprocess.run([test_executable_path], check=True)
        print("--------------------------\n")
    except subprocess.CalledProcessError as e:
        print(f"Test executable failed with error: {e}")
        exit(1)
else:
    print(f"Error: Test executable not found at {test_executable_path}")
    exit(1)


# --- Run the compiled executable ---
executable_path = os.path.join(build_dir, "src", "chtl-compiler")
print(f"Running executable at: {executable_path}")

if os.path.exists(executable_path):
    try:
        # Run the executable and capture its output
        result = subprocess.run([executable_path], check=True, capture_output=True, text=True)
        print("--- Executable Output ---")
        print(result.stdout)
        print("-------------------------")
    except subprocess.CalledProcessError as e:
        print(f"Executable failed with error: {e}")
        print(f"Stderr: {e.stderr}")
else:
    print(f"Error: Executable not found at {executable_path}")