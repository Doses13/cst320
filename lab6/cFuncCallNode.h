#pragma once
//**************************************
// cFuncCallNode.h
//
// Function call node
//

#include "cExprNode.h"
#include "cSymbol.h"
#include "cParamsNode.h"
#include "cDeclNode.h"

class cFuncCallNode : public cExprNode
{
    public:
        cFuncCallNode(cSymbol *name, cParamsNode *params) : cExprNode()
        {
            AddChild(name);
            AddChild(params);

            if (name == nullptr || name->GetDecl() == nullptr)
            {
                string funcName = (name == nullptr) ? string("<null>") : name->GetName();
                SemanticParseError("Symbol " + funcName + " not defined");
            }
        }

        virtual string NodeType() { return string("funcCall"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        cSymbol *GetNameSymbol()
        {
            return static_cast<cSymbol*>(GetChild(0));
        }

        cParamsNode *GetParams()
        {
            return static_cast<cParamsNode*>(GetChild(1));
        }

        cDeclNode *GetDecl()
        {
            cSymbol *sym = GetNameSymbol();
            return (sym == nullptr) ? nullptr : sym->GetDecl();
        }

        virtual cDeclNode *GetType()
        {
            cDeclNode *decl = GetDecl();
            return (decl == nullptr) ? nullptr : decl->GetType();
        }

        string GetName()
        {
            cSymbol *sym = GetNameSymbol();
            return (sym == nullptr) ? string("") : sym->GetName();
        }
};
