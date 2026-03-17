#pragma once
//**************************************
// cArgsNode.h
//
// List of function argument declarations
//

#include "cAstNode.h"
#include "cDeclNode.h"

class cArgsNode : public cAstNode
{
    public:
        cArgsNode(cDeclNode *decl) : cAstNode(), m_size(0)
        {
            AddChild(decl);
        }

        void Insert(cDeclNode *decl)
        {
            AddChild(decl);
        }

        int GetCount()
        {
            return NumChildren();
        }

        cDeclNode *GetArg(int index)
        {
            return static_cast<cDeclNode*>(GetChild(index));
        }

        int GetSize()
        {
            return m_size;
        }

        void SetSize(int size)
        {
            m_size = size;
        }

        virtual string AttributesToString()
        {
            if (m_size == 0) return string("");
            return " size=\"" + std::to_string(m_size) + "\"";
        }

        virtual string NodeType() { return string("args"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    protected:
        int m_size;
};
