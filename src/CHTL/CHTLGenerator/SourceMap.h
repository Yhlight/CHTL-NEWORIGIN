#ifndef CHTL_SOURCE_MAP_H
#define CHTL_SOURCE_MAP_H

#include "../../Common.h"
#include <vector>
#include <sstream>

namespace CHTL {

struct SourceMapping {
    int generatedLine;
    int generatedColumn;
    int originalLine;
    int originalColumn;
    String originalSource;
    String name;
    
    SourceMapping(int genLine = 0, int genCol = 0, int origLine = 0, 
                  int origCol = 0, const String& src = "", const String& n = "")
        : generatedLine(genLine), generatedColumn(genCol), 
          originalLine(origLine), originalColumn(origCol),
          originalSource(src), name(n) {}
};

class SourceMap {
public:
    void addMapping(const SourceMapping& mapping) {
        mappings_.push_back(mapping);
    }
    
    void addMapping(int genLine, int genCol, int origLine, int origCol,
                    const String& source = "", const String& name = "") {
        mappings_.emplace_back(genLine, genCol, origLine, origCol, source, name);
    }
    
    String generate(int version = 3) const {
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"version\": " << version << ",\n";
        ss << "  \"sources\": [";
        
        // Collect unique sources
        std::vector<String> sources;
        for (const auto& mapping : mappings_) {
            if (!mapping.originalSource.empty()) {
                bool found = false;
                for (const auto& src : sources) {
                    if (src == mapping.originalSource) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    sources.push_back(mapping.originalSource);
                }
            }
        }
        
        for (size_t i = 0; i < sources.size(); i++) {
            ss << "\"" << sources[i] << "\"";
            if (i < sources.size() - 1) ss << ", ";
        }
        
        ss << "],\n";
        ss << "  \"names\": [],\n";
        ss << "  \"mappings\": \"" << encodeMappings() << "\"\n";
        ss << "}";
        
        return ss.str();
    }
    
    void clear() {
        mappings_.clear();
    }
    
    const std::vector<SourceMapping>& getMappings() const {
        return mappings_;
    }
    
private:
    std::vector<SourceMapping> mappings_;
    
    String encodeMappings() const {
        // Simple placeholder - full VLQ encoding would be implemented here
        return "";
    }
};

} // namespace CHTL

#endif // CHTL_SOURCE_MAP_H
