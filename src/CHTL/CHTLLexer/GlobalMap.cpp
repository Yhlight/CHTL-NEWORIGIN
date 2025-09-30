#include "CHTL/CHTLLexer/GlobalMap.h"

namespace CHTL {

GlobalMap::GlobalMap() = default;

void GlobalMap::addSymbol(const std::string& name, const std::string& value) {
    symbols_[name] = value;
}

std::string GlobalMap::getSymbol(const std::string& name) const {
    auto it = symbols_.find(name);
    return it != symbols_.end() ? it->second : "";
}

bool GlobalMap::hasSymbol(const std::string& name) const {
    return symbols_.find(name) != symbols_.end();
}

void GlobalMap::removeSymbol(const std::string& name) {
    symbols_.erase(name);
}

void GlobalMap::addKeyword(const std::string& keyword) {
    keywords_[keyword] = true;
}

bool GlobalMap::isKeyword(const std::string& word) const {
    return keywords_.find(word) != keywords_.end();
}

void GlobalMap::addIdentifier(const std::string& identifier) {
    identifiers_[identifier] = true;
}

bool GlobalMap::isIdentifier(const std::string& word) const {
    return identifiers_.find(word) != identifiers_.end();
}

void GlobalMap::clear() {
    symbols_.clear();
    keywords_.clear();
    identifiers_.clear();
}

size_t GlobalMap::getSymbolCount() const {
    return symbols_.size();
}

size_t GlobalMap::getKeywordCount() const {
    return keywords_.size();
}

size_t GlobalMap::getIdentifierCount() const {
    return identifiers_.size();
}

} // namespace CHTL