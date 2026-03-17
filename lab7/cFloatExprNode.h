#pragma once
//**************************************
// cFloatExprNode.h
//
// AST node for a floating-point constant
//

#include "cExprNode.h"
#include "cDeclNode.h"
#include "cSymbol.h"
#include "cSymbolTable.h"

class cFloatExprNode : public cExprNode
{
    public:
        cFloatExprNode(double value) : cExprNode(), m_value(value) {}

        virtual string AttributesToString()
        {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), " value=\"%f\"", m_value);
            return string(buffer);
        }

        virtual string NodeType() { return string("float"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        virtual cDeclNode *GetType()
        {
            cSymbol *sym = g_symbolTable.Find("float");
            return (sym == nullptr) ? nullptr : sym->GetDecl();
        }

    protected:
        double m_value;
};
