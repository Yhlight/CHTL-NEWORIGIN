#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include "../CHTLContext/Context.h"
#include "../CHTLNode/Node.h"
#include <string>
#include <memory>
#include <set>

namespace CHTL {

class Loader {
public:
    Loader();

    // The main entry point for compiling a file
    std::unique_ptr<Node> LoadFile(const std::string& filepath);

    // Called by the parser when it finds an import statement
    void ImportFile(const std::string& filepath);

    Context& getContext() { return context; }

private:
    Context context;
    std::set<std::string> loadedFiles;
};

} // namespace CHTL

#endif // CHTL_LOADER_H
