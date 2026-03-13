#pragma once
//**************************************
// cFuncDeclNode.h
//
// Function declaration / definition node
//

#include "cDeclNode.h"
#include "cSymbol.h"
#include "cArgsNode.h"
#include "cDeclsNode.h"
#include "cStmtsNode.h"

class cFuncDeclNode : public cDeclNode
{
    public:
        cFuncDeclNode(cSymbol *type, cSymbol *name, cArgsNode *args,
                      cDeclsNode *decls, cStmtsNode *stmts) : cDeclNode()
        {
            AddChild(type);   // child 0
            AddChild(name);   // child 1
            AddChild(args);   // child 2
            AddChild(decls);  // child 3
            AddChild(stmts);  // child 4

            if (name == nullptr) return;

            bool thisHadDefinition = (decls != nullptr || stmts != nullptr);

            cDeclNode *oldDecl = name->GetDecl();

            if (oldDecl == nullptr)
            {
                name->SetDecl(this);
                return;
            }

            if (!oldDecl->IsFunc())
            {
                SemanticParseError("Symbol " + name->GetName() +
                    " already defined in current scope");
                return;
            }

            cFuncDeclNode *oldFunc = dynamic_cast<cFuncDeclNode*>(oldDecl);
            if (oldFunc == nullptr)
            {
                SemanticParseError("Symbol " + name->GetName() +
                    " already defined in current scope");
                return;
            }

            bool oldHadDefinition = oldFunc->HasDefinition();

            if (oldFunc->GetType() != GetType())
            {
                SemanticParseError(name->GetName() +
                    " previously declared with different return type");
            }

            if (oldFunc->GetParamCount() != GetParamCount())
            {
                SemanticParseError(name->GetName() +
                    " redeclared with a different number of parameters");
            }

            if (thisHadDefinition && oldHadDefinition)
            {
                SemanticParseError(name->GetName() + " already has a definition");
                return;
            }

            if (!thisHadDefinition && oldHadDefinition)
            {
                SetChild(3, oldFunc->GetDecls());
                SetChild(4, oldFunc->GetStmts());
            }

            name->SetDecl(this);
        }

        virtual bool IsFunc() { return true; }

        virtual string NodeType() { return string("func"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        cSymbol *GetTypeSymbol()
        {
            return static_cast<cSymbol*>(GetChild(0));
        }

        cSymbol *GetNameSymbol()
        {
            return static_cast<cSymbol*>(GetChild(1));
        }

        cArgsNode *GetArgs()
        {
            return static_cast<cArgsNode*>(GetChild(2));
        }

        cDeclsNode *GetDecls()
        {
            return static_cast<cDeclsNode*>(GetChild(3));
        }

        cStmtsNode *GetStmts()
        {
            return static_cast<cStmtsNode*>(GetChild(4));
        }

        bool HasDefinition()
        {
            return GetDecls() != nullptr || GetStmts() != nullptr;
        }

        int GetParamCount()
        {
            cArgsNode *args = GetArgs();
            return (args == nullptr) ? 0 : args->GetCount();
        }

        cDeclNode *GetParamDecl(int index)
        {
            cArgsNode *args = GetArgs();
            return (args == nullptr) ? nullptr : args->GetArg(index);
        }

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
