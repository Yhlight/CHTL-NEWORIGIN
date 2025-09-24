#include "Dispatcher.h"
#include "UnifiedScanner.h"

namespace dispatcher {

DispatchResult CompilerDispatcher::dispatch(std::string_view source) {
    scanner::UnifiedScanner scanner;
    auto segs = scanner.scan(source);
    DispatchResult out;
    for (const auto& s : segs) {
        switch (s.type) {
            case scanner::SegmentType::CHTL: out.chtl.push_back(s.content); break;
            case scanner::SegmentType::CHTL_JS: out.chtljs.push_back(s.content); break;
            case scanner::SegmentType::CSS_PLACEHOLDER: out.cssPlaceholders.push_back(s.content); break;
            case scanner::SegmentType::JS_PLACEHOLDER: out.jsPlaceholders.push_back(s.content); break;
        }
    }
    return out;
}

}

