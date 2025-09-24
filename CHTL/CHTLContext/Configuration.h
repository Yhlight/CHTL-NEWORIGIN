#ifndef CHTL_CONFIGURATION_H
#define CHTL_CONFIGURATION_H

#include <string>
#include <vector>

namespace CHTL {

// Holds all configurable keywords and settings.
// Initialized with default values from the CHTL.md spec.
struct Configuration {
    // General Settings
    int INDEX_INITIAL_COUNT = 0;
    bool DEBUG_MODE = false;
    bool DISABLE_NAME_GROUP = false;
    bool DISABLE_STYLE_AUTO_ADD_CLASS = false;
    bool DISABLE_STYLE_AUTO_ADD_ID = false;
    bool DISABLE_DEFAULT_NAMESPACE = false;

    // Keywords
    std::string KEYWORD_CUSTOM = "[Custom]";
    std::string KEYWORD_TEMPLATE = "[Template]";
    std::string KEYWORD_ORIGIN = "[Origin]";
    std::string KEYWORD_IMPORT = "[Import]";
    std::string KEYWORD_NAMESPACE = "[Namespace]";

    std::string KEYWORD_STYLE = "style";
    std::string KEYWORD_TEXT = "text";

    std::string KEYWORD_FROM = "from";
    // etc... I will not add all of them now, just the ones I need to refactor first.
    // The most important ones are the block keywords like [Template].
};

} // namespace CHTL

#endif // CHTL_CONFIGURATION_H
