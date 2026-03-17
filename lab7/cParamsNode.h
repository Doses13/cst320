#pragma once
//**************************************
// cParamsNode.h
//
// List of function call parameters
//

#include "cAstNode.h"
#include "cExprNode.h"

class cParamsNode : public cAstNode
{
    public:
        cParamsNode(cExprNode *expr) : cAstNode(), m_size(0)
        {
            AddChild(expr);
        }

        void Insert(cExprNode *expr)
        {
            AddChild(expr);
        }

        int GetCount()
        {
            return NumChildren();
        }

        cExprNode *GetParam(int index)
        {
            return static_cast<cExprNode*>(GetChild(index));
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

        virtual string NodeType() { return string("params"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

    protected:
        int m_size;
};
