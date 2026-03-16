#pragma once
//**************************************
// cIntExprNode.h
//
// Defines an AST node for an integer constant (literals).
//

#include "cExprNode.h"
#include "cDeclNode.h"
#include "cSymbol.h"
#include "cSymbolTable.h"

class cIntExprNode : public cExprNode
{
    public:
        cIntExprNode(int value) : cExprNode(), m_value(value) {}

        virtual string AttributesToString()
        {
            return " value=\"" + std::to_string(m_value) + "\"";
        }

        virtual string NodeType() { return string("int"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        virtual cDeclNode *GetType()
        {
            const char *typeName = (m_value >= 0 && m_value <= 127) ? "char" : "int";
            cSymbol *sym = g_symbolTable.Find(typeName);
            return (sym == nullptr) ? nullptr : sym->GetDecl();
        }

    protected:
        int m_value;
};
