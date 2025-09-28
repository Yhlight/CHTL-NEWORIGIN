#include "ResponsiveValueProcessor.h"
#include <regex>

void ResponsiveValueProcessor::process(std::string& source, const SharedContext& context) {
    if (context.responsive_variable_map.empty()) {
        return;
    }

    std::string proxy_handler_template = R"JS(
const handler_$$VAR_NAME$$ = {
    set: function(target, property, value) {
        target[property] = value;
        const element = document.getElementById('$$ELEMENT_ID$$');
        if (element) {
            element.innerHTML = value;
        }
        return true;
    }
};
)JS";

    for (const auto& pair : context.responsive_variable_map) {
        const std::string& var_name = pair.first;
        const std::string& element_id = pair.second;

        // Find the variable declaration (e.g., "let myVar = 'initial';")
        std::regex decl_regex("(let|var|const)\\s+" + var_name + "\\s*=\\s*(.*?);");
        std::smatch match;

        if (std::regex_search(source, match, decl_regex)) {
            std::string initial_value = match[2].str();

            // Create the specific handler for this variable
            std::string proxy_handler = proxy_handler_template;
            proxy_handler = std::regex_replace(proxy_handler, std::regex("\\$\\$VAR_NAME\\$\\$"), var_name);
            proxy_handler = std::regex_replace(proxy_handler, std::regex("\\$\\$ELEMENT_ID\\$\\$"), element_id);

            // Create the proxy declaration
            std::string proxy_decl = "let " + var_name + " = new Proxy({ value: " + initial_value + " }, handler_" + var_name + ");";

            // Inject the handler and replace the original declaration
            source.insert(0, proxy_handler);
            source = std::regex_replace(source, decl_regex, proxy_decl);
        }
    }
}