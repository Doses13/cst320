#pragma once
//**************************************
// cIfNode.h
//
// AST node for if statements
//

#include "cStmtNode.h"
#include "cExprNode.h"
#include "cStmtsNode.h"

class cIfNode : public cStmtNode
{
    public:
        cIfNode(cExprNode *cond, cStmtsNode *thenPart) : cStmtNode()
        {
            AddChild(cond);
            AddChild(thenPart);
        }

        cIfNode(cExprNode *cond, cStmtsNode *thenPart, cStmtsNode *elsePart) : cStmtNode()
        {
            AddChild(cond);
            AddChild(thenPart);
            AddChild(elsePart);
        }

        virtual string NodeType() { return string("if"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        cExprNode *GetCond()
        {
            return static_cast<cExprNode*>(GetChild(0));
        }

        cStmtsNode *GetThenPart()
        {
            return static_cast<cStmtsNode*>(GetChild(1));
        }

        cStmtsNode *GetElsePart()
        {
            return static_cast<cStmtsNode*>(GetChild(2));
        }
};