#include "Arg.h"

// Placeholder implementations
void Arg::fillValue(const Arg& scanned_arg) {
    for (size_t i = 0; i < arguments.size() && i < scanned_arg.arguments.size(); ++i) {
        arguments[i]->fillValue(scanned_arg.arguments[i]->value);
    }
}

std::string Arg::match(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
    // A real implementation would be more sophisticated, using the match_counters
    // to find the correct instance of a repeated placeholder.
    for (const auto& arg : arguments) {
        if (arg->placeholder == placeholder) {
            return func(arg->value);
        }
    }
    return "";
}

void Arg::transform(const std::string& final_code) {
    this->transformed_code = final_code;
}

const std::string& Arg::getTransformation() const {
    return this->transformed_code;
}