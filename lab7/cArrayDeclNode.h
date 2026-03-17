#pragma once
//**************************************
// cArrayDeclNode.h
//
// AST node for array declarations
//

#include "cDeclNode.h"
#include "cSymbol.h"

class cArrayDeclNode : public cDeclNode
{
    public:
        cArrayDeclNode(cSymbol *type, cSymbol *name, int count) : cDeclNode(), m_count(count)
        {
            AddChild(type);
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

        virtual bool IsArray() { return true; }
        virtual bool IsVar()   { return true; }
        virtual bool IsType()  { return false; }

        virtual string NodeType() { return string("array_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        virtual string AttributesToString()
        {
            return " count=\"" + std::to_string(m_count) + "\"";
        }

        cSymbol *GetTypeSymbol()
        {
            return static_cast<cSymbol*>(GetChild(0));
        }

        cSymbol *GetNameSymbol()
        {
            return static_cast<cSymbol*>(GetChild(1));
        }

        int GetCount() { return m_count; }

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

    protected:
        int m_count;
};
