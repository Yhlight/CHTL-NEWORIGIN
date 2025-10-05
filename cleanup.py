import os
import shutil

# This script is to remove the third-party directory which contains non-ASCII filenames
# that cause issues with the standard rm command in this environment.
try:
    shutil.rmtree('third-party')
    print("Successfully removed third-party directory.")
except Exception as e:
    print(f"Error removing third-party directory: {e}")