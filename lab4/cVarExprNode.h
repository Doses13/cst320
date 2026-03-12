#pragma once
//**************************************
// cVarExprNode.h
//
// AST node for a variable reference
//

#include "cExprNode.h"
#include "cSymbol.h"

class cVarExprNode : public cExprNode
{
    public:
        cVarExprNode(cSymbol *sym) : cExprNode()
        {
            AddChild(sym);
        }

        void AddPart(cAstNode *node)
        {
            AddChild(node);
        }

        virtual string NodeType() { return string("varref"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        cSymbol *GetSymbol()
        {
            return static_cast<cSymbol*>(GetChild(0));
        }
};