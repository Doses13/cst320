#pragma once
//**************************************
// cPrintsNode.h
//
// AST node for printing a string literal
//

#include "cStmtNode.h"

class cPrintsNode : public cStmtNode
{
    public:
        cPrintsNode(const std::string &value) : cStmtNode()
        {
            m_value = value;
        }

        virtual string NodeType() { return string("prints"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        virtual string AttributesToString()
        {
            return " value=\"" + m_value + "\"";
        }

    protected:
        std::string m_value;
};