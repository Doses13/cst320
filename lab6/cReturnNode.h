#pragma once
//**************************************
// cReturnNode.h
//
// AST node for return statements
//

#include "cStmtNode.h"
#include "cExprNode.h"

class cReturnNode : public cStmtNode
{
    public:
        cReturnNode(cExprNode *expr) : cStmtNode()
        {
            AddChild(expr);
        }

        virtual string NodeType() { return string("return"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};