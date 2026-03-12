#pragma once
//**************************************
// cSymbol.h
//
// Defines class used to represent symbols.
//

#include <string>
using std::string;

#include "cAstNode.h"

class cSymbol : public cAstNode
{
    public:
        cSymbol(string name) : cAstNode()
        {
            m_id = ++nextId;
            m_name = name;
            m_isType = false;
        }

        string GetName() { return m_name; }

        bool IsType() const { return m_isType; }
        void SetType(bool isType = true) { m_isType = isType; }

        virtual string AttributesToString()
        {
            string result(" id=\"");
            result += std::to_string(m_id);
            result += "\" name=\"" + m_name + "\"";
            return result;
        }

        virtual string NodeType() { return string("sym"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    protected:
        static long long nextId;
        long long m_id;
        string m_name;
        bool m_isType;
};