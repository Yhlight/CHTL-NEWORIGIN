from CompilerDispatcher.dispatcher import CompilerDispatcher

def main():
    try:
        with open("test.chtl", "r") as f:
            source_code = f.read()
    except FileNotFoundError:
        print("Error: test.chtl not found.")
        return

    dispatcher = CompilerDispatcher()
    final_output = dispatcher.dispatch(source_code)

    print("\\n---- Final Output from test.chtl ----")
    print(final_output)

if __name__ == "__main__":
    main()