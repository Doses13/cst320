#pragma once
//**************************************
// cFuncCallNode.h
//
// Function call node
//

#include "cExprNode.h"
#include "cSymbol.h"
#include "cParamsNode.h"

class cFuncCallNode : public cExprNode
{
    public:
        cFuncCallNode(cSymbol *name, cParamsNode *params) : cExprNode()
        {
            AddChild(name);
            AddChild(params);
        }

        virtual string NodeType() { return string("funcCall"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};