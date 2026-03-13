#pragma once
//**************************************
// cParamsNode.h
//
// List of function call parameters
//

#include "cAstNode.h"
#include "cExprNode.h"

class cParamsNode : public cAstNode
{
    public:
        cParamsNode(cExprNode *expr) : cAstNode()
        {
            AddChild(expr);
        }

        void Insert(cExprNode *expr)
        {
            AddChild(expr);
        }

        int GetCount()
        {
            return NumChildren();
        }

        cExprNode *GetParam(int index)
        {
            return static_cast<cExprNode*>(GetChild(index));
        }

        virtual string NodeType() { return string("params"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
