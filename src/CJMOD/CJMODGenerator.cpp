#include "CJMODGenerator.h"
#include "CJMODResultQueue.h" // Include the new queue header
#include <iostream>

// The exportResult function now adds the transformed code from the Arg object
// to the singleton queue, making it available to the main compiler.
void CJMODGenerator::exportResult(const Arg& result_arg) {
    CJMODResultQueue::getInstance().addResult(result_arg.getTransformation());
}