#pragma once
//**************************************
// cWhileNode.h
//
// AST node for while statements
//

#include "cStmtNode.h"
#include "cExprNode.h"

class cWhileNode : public cStmtNode
{
    public:
        cWhileNode(cExprNode *cond, cStmtNode *stmt) : cStmtNode()
        {
            AddChild(cond);
            AddChild(stmt);
        }

        virtual string NodeType() { return string("while"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        cExprNode *GetCond()
        {
            return static_cast<cExprNode*>(GetChild(0));
        }

        cStmtNode *GetStmt()
        {
            return static_cast<cStmtNode*>(GetChild(1));
        }
};