#pragma once
//**************************************
// cBaseTypeNode
//
// Defines base class for built-in type declarations.
//

#include "cDeclNode.h"

class cBaseTypeNode : public cDeclNode
{
    public:
        cBaseTypeNode(string name, int size, bool isFloat)
            : cDeclNode(), m_name(name), m_size(size), m_isFloat(isFloat)
        {
        }

        virtual bool IsFloat() { return m_isFloat; }
        virtual bool IsInt()   { return !m_isFloat; }
        virtual bool IsChar()  { return (!m_isFloat && m_size == 1); }
        virtual bool IsType()  { return true; }

        virtual cDeclNode *GetType() { return this; }
        virtual string GetName() { return m_name; }

        virtual string NodeType() { return "type"; }
        virtual string AttributesToString()
        {
            return " name=\"" + m_name +
                   "\" size=\"" + std::to_string(m_size) +
                   "\" isFloat=\"" + std::to_string(m_isFloat) + "\"";
        }

        virtual int GetSize() { return m_size; }

        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    protected:
        string m_name;
        int    m_size;
        bool   m_isFloat;
};
