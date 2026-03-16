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
            if (name->GetDecl() != nullptr)
            {
                SemanticParseError("Symbol " + name->GetName() + " already defined in current scope");
            }
            else
            {
                name->SetDecl(this);
            }
        }

        virtual bool IsVar() { return true; }

        virtual string NodeType() { return string("var_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        cSymbol *GetTypeSymbol() { return static_cast<cSymbol*>(GetChild(0)); }
        cSymbol *GetNameSymbol() { return static_cast<cSymbol*>(GetChild(1)); }

        virtual cDeclNode *GetType()
        {
            cSymbol *typeSym = GetTypeSymbol();
            return (typeSym == nullptr) ? nullptr : typeSym->GetDecl();
        }

        virtual string GetName()
        {
            cSymbol *nameSym = GetNameSymbol();
            return (nameSym == nullptr) ? string("") : nameSym->GetName();
        }
};
