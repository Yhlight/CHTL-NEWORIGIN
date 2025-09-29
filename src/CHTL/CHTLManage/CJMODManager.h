#pragma once

#include <string>
#include <functional>
#include <map>
#include <memory>
#include "CJMOD/Arg.h"

// A simplified API object passed to the CJMOD's registration function.
// This allows the module to register its syntax transformations with the compiler.
struct CJMOD_API {
    // Maps a function name (e.g., "printMylove") to a transformation function.
    // The transform function takes the parsed arguments and returns the generated JS string.
    std::map<std::string, std::function<std::string(Arg&)>>* syntax_transformers;
};

class CJMODManager {
public:
    CJMODManager();
    ~CJMODManager();

    // Loads a CJMOD shared library from the given path.
    void load_module(const std::string& path);

    // Checks if a syntax transformer for a given function name exists.
    bool has_transformer(const std::string& name);

    // Executes the transformer for a given function name with the given arguments.
    std::string execute_transformer(const std::string& name, Arg& args);

private:
    // A map to store the registered transformation functions from all loaded modules.
    std::map<std::string, std::function<std::string(Arg&)>> transformers;

    // A list of handles to the loaded shared libraries, to be closed on destruction.
    std::vector<void*> library_handles;
};