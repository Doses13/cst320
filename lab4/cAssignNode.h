#pragma once
//**************************************
// cAssignNode.h
//
// AST node for assignment statements
//

#include "cStmtNode.h"
#include "cExprNode.h"

class cAssignNode : public cStmtNode
{
    public:
        cAssignNode(cExprNode *lhs, cExprNode *rhs) : cStmtNode()
        {
            AddChild(lhs);
            AddChild(rhs);
        }

        virtual string NodeType() { return string("assign"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};