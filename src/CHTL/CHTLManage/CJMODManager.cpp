#include "CJMODManager.h"
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

// Define a common type for the registration function pointer.
typedef void (*register_cjmod_func)(CJMOD_API*);

CJMODManager::CJMODManager() = default;

CJMODManager::~CJMODManager() {
    // Unload all the libraries when the manager is destroyed.
    for (void* handle : library_handles) {
        if (handle) {
#ifdef _WIN32
            FreeLibrary(static_cast<HMODULE>(handle));
#else
            dlclose(handle);
#endif
        }
    }
}

void CJMODManager::load_module(const std::string& path) {
    void* handle = nullptr;

#ifdef _WIN32
    handle = LoadLibrary(path.c_str());
#else
    handle = dlopen(path.c_str(), RTLD_LAZY);
#endif

    if (!handle) {
        throw std::runtime_error("Failed to load CJMOD module: " + path);
    }

    // Look for the registration function in the loaded library.
    register_cjmod_func register_func = nullptr;
#ifdef _WIN32
    register_func = (register_cjmod_func)GetProcAddress(static_cast<HMODULE>(handle), "register_cjmod");
#else
    register_func = (register_cjmod_func)dlsym(handle, "register_cjmod");
#endif

    if (!register_func) {
#ifdef _WIN32
        FreeLibrary(static_cast<HMODULE>(handle));
#else
        dlclose(handle);
#endif
        throw std::runtime_error("CJMOD module does not export 'register_cjmod' function: " + path);
    }

    // Create the API object and call the registration function.
    CJMOD_API api;
    api.syntax_transformers = &this->transformers;
    register_func(&api);

    library_handles.push_back(handle);
}

bool CJMODManager::has_transformer(const std::string& name) {
    return transformers.count(name) > 0;
}

std::string CJMODManager::execute_transformer(const std::string& name, Arg& args) {
    if (!has_transformer(name)) {
        throw std::runtime_error("No CJMOD transformer registered for function: " + name);
    }
    return transformers.at(name)(args);
}