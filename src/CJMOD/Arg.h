#pragma once

#include "AtomArg.h"
#include <vector>
#include <string>
#include <memory>
#include <map>

// Represents a collection of atomic arguments parsed from a syntax pattern.
// This is the main object that module developers will interact with.
class Arg {
public:
    std::vector<std::unique_ptr<AtomArg>> arguments;

    // Fills the values of this Arg object's atoms from another Arg object
    // that presumably holds the scanned values from real code.
    void fillValue(const Arg& scanned_arg);

    // Matches a placeholder in the argument list and applies a function to its value.
    // Keeps track of which placeholder it has matched (e.g., the first '$', the second '$').
    std::string match(const std::string& placeholder, std::function<std::string(const std::string&)> func);

    // Sets the final transformation for this entire argument list, which will be
    // the resulting JavaScript code.
    void transform(const std::string& final_code);
    const std::string& getTransformation() const;

private:
    std::string transformed_code;
    std::map<std::string, int> match_counters;
};