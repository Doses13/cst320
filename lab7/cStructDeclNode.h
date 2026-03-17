#pragma once
//**************************************
// cStructDeclNode.h
//
// AST node for struct declarations
//

#include "cDeclNode.h"
#include "cDeclsNode.h"
#include "cSymbol.h"

class cStructDeclNode : public cDeclNode
{
    public:
        cStructDeclNode(cDeclsNode *fields, cSymbol *name) : cDeclNode()
        {
            AddChild(fields);
            AddChild(name);

            if (name != nullptr)
            {
                if (name->GetDecl() != nullptr)
                {
                    SemanticParseError("Symbol " + name->GetName() +
                        " already defined in current scope");
                }
                else
                {
                    name->SetDecl(this);
                }
            }
        }

        virtual bool IsStruct() { return true; }
        virtual bool IsType()   { return true; }

        virtual string NodeType() { return string("struct_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        cDeclsNode *GetFields()
        {
            return static_cast<cDeclsNode*>(GetChild(0));
        }

        cSymbol *GetNameSymbol()
        {
            return static_cast<cSymbol*>(GetChild(1));
        }

        virtual cDeclNode *GetType() { return this; }

        virtual string GetName()
        {
            cSymbol *nameSym = GetNameSymbol();
            return (nameSym == nullptr) ? string("") : nameSym->GetName();
        }
};