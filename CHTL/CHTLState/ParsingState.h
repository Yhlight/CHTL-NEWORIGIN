#ifndef CHTL_PARSING_STATE_H
#define CHTL_PARSING_STATE_H

#include "State.h"
#include "../CHTLStrategy/ParseElementStrategy.h"
#include "../CHTLStrategy/ParseTemplateStrategy.h"
#include "../CHTLStrategy/ParseImportStrategy.h"
#include "../CHTLStrategy/ParseNamespaceStrategy.h"
#include <memory>

namespace CHTL {

class ParsingState : public State {
public:
    ParsingState();
    void Handle(Context* context, Parser& parser) override;
private:
    std::unique_ptr<ParseElementStrategy> parseElementStrategy;
    std::unique_ptr<ParseTemplateStrategy> parseTemplateStrategy;
    std::unique_ptr<ParseImportStrategy> parseImportStrategy;
    std::unique_ptr<ParseNamespaceStrategy> parseNamespaceStrategy;
};

} // namespace CHTL

#endif // CHTL_PARSING_STATE_H
