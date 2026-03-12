#pragma once
//**************************************
// cVarDeclNode.h
//
// AST node for a variable declaration
//

#include "cDeclNode.h"
#include "cSymbol.h"

class cVarDeclNode : public cDeclNode
{
    public:
        cVarDeclNode(cSymbol *type, cSymbol *name) : cDeclNode()
        {
            AddChild(type);
            AddChild(name);
        }

        virtual string NodeType() { return string("var_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        cSymbol *GetType() { return static_cast<cSymbol*>(GetChild(0)); }
        cSymbol *GetName() { return static_cast<cSymbol*>(GetChild(1)); }
};