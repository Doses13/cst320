#pragma once
//**************************************
// cExprNode.h
//
// Defines base class for all expressions
//

#include "cStmtNode.h"

class cDeclNode;

class cExprNode : public cStmtNode
{
    public:
        cExprNode() : cStmtNode() {}
        virtual cDeclNode *GetType() = 0;
};
