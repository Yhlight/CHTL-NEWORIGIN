import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python write_file.py <filepath> <content>")
        sys.exit(1)

    filepath = sys.argv[1]
    content = sys.argv[2]

    try:
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"Successfully wrote to {filepath}")
    except Exception as e:
        print(f"Error writing to file: {e}")
        sys.exit(1)