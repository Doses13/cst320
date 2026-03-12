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
        cArrayDeclNode(cSymbol *type, cSymbol *name, int count) : cDeclNode()
        {
            m_count = count;
            AddChild(type);
            AddChild(name);
        }

        virtual string NodeType() { return string("array_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        virtual string AttributesToString()
        {
            return " count=\"" + std::to_string(m_count) + "\"";
        }

    protected:
        int m_count;
};