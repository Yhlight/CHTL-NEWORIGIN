#ifndef CHTL_STYLE_BLOCK_STATE_H
#define CHTL_STYLE_BLOCK_STATE_H

#include "IState.h"
#include "../CHTLNode/StyleNode.h"

namespace CHTL
{
    namespace State
    {
        class StyleBlockState : public IState
        {
        public:
            StyleBlockState(AST::StyleNode* ownerNode);

            void Handle(Parser& parser) override;

        private:
            AST::StyleNode* m_ownerNode;
        };
    }
}

#endif // CHTL_STYLE_BLOCK_STATE_H
