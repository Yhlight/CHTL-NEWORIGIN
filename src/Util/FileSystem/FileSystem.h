#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>

namespace CHTL {
    class FileSystem {
    public:
        static std::string ReadFile(const std::string& path);
    };
}

#endif // FILESYSTEM_H
