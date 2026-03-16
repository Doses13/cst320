#pragma once
//**************************************
// cSymbol.h
//
// Defines class used to represent symbols.
//

#include <string>
using std::string;

#include "cAstNode.h"

class cDeclNode;

class cSymbol : public cAstNode
{
    public:
        cSymbol(string name) : cAstNode(), m_decl(nullptr)
        {
            m_id = ++nextId;
            m_name = name;
        }

        string GetName() { return m_name; }

        cDeclNode *GetDecl() { return m_decl; }
        void SetDecl(cDeclNode *decl) { m_decl = decl; }

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
        cDeclNode *m_decl;
};
