#ifndef CHTL_WEB_COMPONENTS_SUPPORT_H
#define CHTL_WEB_COMPONENTS_SUPPORT_H

#include "../../Common.h"
#include <sstream>

namespace CHTL {

struct WebComponentConfig {
    String tagName;
    String className;
    Vector<String> observedAttributes;
    bool useShadowDOM = true;
    String shadowMode = "open"; // "open" or "closed"
    bool delegatesFocus = false;
};

class WebComponentsGenerator {
public:
    static String generateCustomElement(const WebComponentConfig& config, 
                                        const String& template_html,
                                        const String& styles,
                                        const String& script) {
        std::stringstream ss;
        
        ss << "class " << config.className << " extends HTMLElement {\n";
        ss << "  constructor() {\n";
        ss << "    super();\n";
        
        if (config.useShadowDOM) {
            ss << "    this.attachShadow({ mode: '" << config.shadowMode << "'";
            if (config.delegatesFocus) {
                ss << ", delegatesFocus: true";
            }
            ss << " });\n";
        }
        
        ss << "  }\n\n";
        
        // connectedCallback
        ss << "  connectedCallback() {\n";
        if (config.useShadowDOM) {
            ss << "    this.shadowRoot.innerHTML = `\n";
            if (!styles.empty()) {
                ss << "      <style>" << styles << "</style>\n";
            }
            ss << "      " << template_html << "\n";
            ss << "    `;\n";
        } else {
            ss << "    this.innerHTML = `" << template_html << "`;\n";
        }
        ss << "    " << script << "\n";
        ss << "  }\n\n";
        
        // observedAttributes
        if (!config.observedAttributes.empty()) {
            ss << "  static get observedAttributes() {\n";
            ss << "    return [";
            for (size_t i = 0; i < config.observedAttributes.size(); i++) {
                ss << "'" << config.observedAttributes[i] << "'";
                if (i < config.observedAttributes.size() - 1) ss << ", ";
            }
            ss << "];\n";
            ss << "  }\n\n";
            
            // attributeChangedCallback
            ss << "  attributeChangedCallback(name, oldValue, newValue) {\n";
            ss << "    // Handle attribute changes\n";
            ss << "  }\n\n";
        }
        
        // disconnectedCallback
        ss << "  disconnectedCallback() {\n";
        ss << "    // Cleanup\n";
        ss << "  }\n";
        
        ss << "}\n\n";
        
        // Register custom element
        ss << "customElements.define('" << config.tagName << "', " << config.className << ");\n";
        
        return ss.str();
    }
    
    static String generateShadowDOMStyles(const String& styles) {
        std::stringstream ss;
        ss << ":host {\n";
        ss << "  display: block;\n";
        ss << "}\n";
        ss << styles;
        return ss.str();
    }
};

} // namespace CHTL

#endif // CHTL_WEB_COMPONENTS_SUPPORT_H
