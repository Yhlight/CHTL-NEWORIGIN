#include "UnifiedScanner.hpp"

namespace chtl::scanner {

std::vector<Fragment> UnifiedScanner::scan(const std::string& source) const {
    // Minimal stub: treat whole as CHTL for now. Placeholder logic will be added per spec.
    return { Fragment{ FragmentType::Chtl, source } };
}

}
