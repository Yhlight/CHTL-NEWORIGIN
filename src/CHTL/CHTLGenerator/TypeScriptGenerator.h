#ifndef CHTL_TYPESCRIPT_GENERATOR_H
#define CHTL_TYPESCRIPT_GENERATOR_H

#include "../../Common.h"
#include <sstream>

namespace CHTL {

class TypeScriptGenerator {
public:
    static String generateTypeDefinitions(const String& componentName,
                                          const Vector<String>& attributes,
                                          const HashMap<String, String>& properties) {
        std::stringstream ss;
        
        // Interface for component
        ss << "export interface " << componentName << "Element extends HTMLElement {\n";
        
        // Attributes
        for (const auto& attr : attributes) {
            ss << "  " << attr << "?: string;\n";
        }
        
        // Properties
        for (const auto& [name, type] : properties) {
            ss << "  " << name << ": " << type << ";\n";
        }
        
        ss << "}\n\n";
        
        // JSX types
        ss << "declare global {\n";
        ss << "  namespace JSX {\n";
        ss << "    interface IntrinsicElements {\n";
        ss << "      '" << componentName << "': Partial<" << componentName << "Element>;\n";
        ss << "    }\n";
        ss << "  }\n";
        ss << "}\n\n";
        
        // Constructor
        ss << "export declare const " << componentName << ": {\n";
        ss << "  new(): " << componentName << "Element;\n";
        ss << "  prototype: " << componentName << "Element;\n";
        ss << "};\n";
        
        return ss.str();
    }
    
    static String inferType(const String& value) {
        // Simple type inference
        if (value == "true" || value == "false") return "boolean";
        if (!value.empty() && std::isdigit(value[0])) {
            if (value.find('.') != String::npos) return "number";
            return "number";
        }
        if (value[0] == '"' || value[0] == '\'') return "string";
        if (value[0] == '[') return "Array<any>";
        if (value[0] == '{') return "Record<string, any>";
        return "any";
    }
};

} // namespace CHTL

#endif // CHTL_TYPESCRIPT_GENERATOR_H
