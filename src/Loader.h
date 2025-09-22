#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>
#include <vector>
#include <set>

class Loader {
public:
    std::string load(const std::string& main_filepath);

private:
    std::string load_and_preprocess(const std::string& filepath, std::set<std::string>& loaded_files);
    std::string get_base_path(const std::string& filepath);
};

#endif //CHTL_LOADER_H
