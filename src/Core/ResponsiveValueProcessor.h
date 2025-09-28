#pragma once

#include "SharedContext.h"
#include <string>

class ResponsiveValueProcessor {
public:
    void process(std::string& source, const SharedContext& context);
};