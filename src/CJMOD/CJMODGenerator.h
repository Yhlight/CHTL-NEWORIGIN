#pragma once

#include "Arg.h"
#include <string>

// The CJMODGenerator provides an interface for custom modules to export their
// final generated JavaScript back to the main CHTL JS compiler.
class CJMODGenerator {
public:
    // Exports the final transformed code from an Arg object.
    static void exportResult(const Arg& result_arg);
};