#pragma once
//**************************************
// cSemantics.h
//
// Semantic checker visitor for Lab 5B
//

#include "cVisitor.h"

class cAssignNode;
class cVarExprNode;
class cFuncCallNode;

class cSemantics : public cVisitor
{
    public:
        virtual void Visit(cAssignNode *node) override;
        virtual void Visit(cVarExprNode *node) override;
        virtual void Visit(cFuncCallNode *node) override;
};
