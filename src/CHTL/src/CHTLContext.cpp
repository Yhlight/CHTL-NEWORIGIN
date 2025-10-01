#include "CHTLContext.h"
#include <sstream>

namespace CHTL {

CHTLContext::CHTLContext()
    : debugMode_(false) {
}

CHTLContext::~CHTLContext() {
}

void CHTLContext::setInputFile(const std::string& filepath) {
    inputFile_ = filepath;
}

void CHTLContext::setOutputDirectory(const std::string& dir) {
    outputDirectory_ = dir;
}

void CHTLContext::setConfiguration(const std::string& key, const std::string& value) {
    configuration_[key] = value;
}

std::string CHTLContext::getConfiguration(const std::string& key) const {
    auto it = configuration_.find(key);
    return (it != configuration_.end()) ? it->second : "";
}

void CHTLContext::addModule(const std::string& name, const std::string& path) {
    modules_[name] = path;
}

std::string CHTLContext::getModulePath(const std::string& name) const {
    auto it = modules_.find(name);
    return (it != modules_.end()) ? it->second : "";
}

void CHTLContext::addError(const std::string& message, int line, int column) {
    std::ostringstream oss;
    if (line >= 0 && column >= 0) {
        oss << "Error at line " << line << ", column " << column << ": " << message;
    } else if (line >= 0) {
        oss << "Error at line " << line << ": " << message;
    } else {
        oss << "Error: " << message;
    }
    errors_.push_back(oss.str());
}

void CHTLContext::addWarning(const std::string& message, int line, int column) {
    std::ostringstream oss;
    if (line >= 0 && column >= 0) {
        oss << "Warning at line " << line << ", column " << column << ": " << message;
    } else if (line >= 0) {
        oss << "Warning at line " << line << ": " << message;
    } else {
        oss << "Warning: " << message;
    }
    warnings_.push_back(oss.str());
}

void CHTLContext::clearErrors() {
    errors_.clear();
}

} // namespace CHTL