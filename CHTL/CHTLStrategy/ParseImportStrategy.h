#ifndef CHTL_PARSE_IMPORT_STRATEGY_H
#define CHTL_PARSE_IMPORT_STRATEGY_H

#include "Strategy.h"

namespace CHTL {

class ParseImportStrategy : public Strategy {
public:
    void Execute(Context* context, Parser& parser) override;
};

} // namespace CHTL

#endif // CHTL_PARSE_IMPORT_STRATEGY_H
