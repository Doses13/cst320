#pragma once
//**************************************
// cVarExprNode.h
//
// AST node for a variable reference
//

#include "cExprNode.h"
#include "cSymbol.h"
#include "cDeclNode.h"

class cVarExprNode : public cExprNode
{
    public:
        cVarExprNode(cSymbol *sym) : cExprNode()
        {
            AddChild(sym);
            
            if (sym == nullptr || sym->GetDecl() == nullptr)
            {
                string name = (sym == nullptr) ? string("<null>") : sym->GetName();
                SemanticParseError("Symbol " + name + " not defined");
            }
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

        string GetName()
        {
            cSymbol *sym = GetSymbol();
            return (sym == nullptr) ? string("") : sym->GetName();
        }

        int GetPartCount()
        {
            int children = NumChildren();
            return (children <= 1) ? 0 : (children - 1);
        }

        cExprNode *GetPart(int index)
        {
            return static_cast<cExprNode*>(GetChild(index + 1));
        }

        virtual cDeclNode *GetDecl()
        {
            cSymbol *sym = GetSymbol();
            return (sym == nullptr) ? nullptr : sym->GetDecl();
        }

        virtual cDeclNode *GetType()
        {
            cDeclNode *decl = GetDecl();
            if (decl == nullptr) return nullptr;

            cDeclNode *type = decl->GetType();
            if (type == nullptr) return nullptr;

            for (int i = 0; i < GetPartCount(); i++)
            {
                if (!type->IsArray()) return type;
                type = type->GetType();
                if (type == nullptr) return nullptr;
            }

            return type;
        }
};
