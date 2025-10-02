import os
import subprocess
import shutil

# Clean and create build directory
if os.path.exists("build"):
    shutil.rmtree("build")
os.makedirs("build")

# Change to the build directory
os.chdir("build")

# Run cmake and make
subprocess.run(["cmake", ".."], check=True)
subprocess.run(["make"], check=True)