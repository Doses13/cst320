#pragma once
//**************************************
// cBinaryExprNode.h
//
// AST node for a binary expression like a+b or x*y
//

#include "cExprNode.h"
#include "cOpNode.h"

class cBinaryExprNode : public cExprNode
{
    public:
        cBinaryExprNode(cExprNode *left, cOpNode *op, cExprNode *right)
            : cExprNode()
        {
            AddChild(left);
            AddChild(op);
            AddChild(right);
        }

        virtual string NodeType() { return string("expr"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        cExprNode *GetLeft()  { return static_cast<cExprNode*>(GetChild(0)); }
        cOpNode   *GetOp()    { return static_cast<cOpNode*>(GetChild(1)); }
        cExprNode *GetRight() { return static_cast<cExprNode*>(GetChild(2)); }
};