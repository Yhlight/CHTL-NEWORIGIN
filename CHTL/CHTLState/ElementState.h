#ifndef CHTL_ELEMENT_STATE_H
#define CHTL_ELEMENT_STATE_H

#include "IState.h"
#include "../CHTLNode/ElementNode.h"

namespace CHTL
{
    namespace State
    {
        class ElementState : public IState
        {
        public:
            ElementState(AST::ElementNode* ownerNode);

            void Handle(Parser& parser) override;

        private:
            AST::ElementNode* m_ownerNode;
            void ParseAttribute(Parser& parser);
        };
    }
}

#endif // CHTL_ELEMENT_STATE_H
