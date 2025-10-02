/**
 * @file CHTLContext.cpp
 * @brief Implementation of CHTL Compilation Context
 */

#include "CHTLContext.h"

namespace CHTL {

CHTLContext::CHTLContext()
    : debugMode_(false)
    , inlineMode_(false)
    , inlineCss_(false)
    , inlineJs_(false)
    , defaultStruct_(false)
    , indexInitialCount_(0)
{
}

CHTLContext::~CHTLContext() {
}

void CHTLContext::enterNamespace(const std::string& name) {
    if (currentNamespace_.empty()) {
        currentNamespace_ = name;
    } else {
        currentNamespace_ += "." + name;
    }
}

void CHTLContext::exitNamespace() {
    size_t pos = currentNamespace_.find_last_of('.');
    if (pos != std::string::npos) {
        currentNamespace_ = currentNamespace_.substr(0, pos);
    } else {
        currentNamespace_.clear();
    }
}

std::string CHTLContext::getCurrentNamespace() const {
    return currentNamespace_;
}

void CHTLContext::registerSymbol(const std::string& name, const std::string& type) {
    std::string fullName = currentNamespace_.empty() ? name : currentNamespace_ + "." + name;
    symbolTable_[fullName] = type;
}

bool CHTLContext::hasSymbol(const std::string& name) const {
    std::string fullName = currentNamespace_.empty() ? name : currentNamespace_ + "." + name;
    return symbolTable_.find(fullName) != symbolTable_.end();
}

} // namespace CHTL
