#pragma once
//**************************************
// cDeclNode.h
//
// Base class for all declarations.
//

#include "cAstNode.h"

class cDeclNode : public cAstNode
{
    public:
        cDeclNode() : cAstNode(), m_nodeSize(0), m_nodeOffset(0) {}

        virtual bool IsArray()  { return false; }
        virtual bool IsStruct() { return false; }
        virtual bool IsType()   { return false; }
        virtual bool IsFunc()   { return false; }
        virtual bool IsVar()    { return false; }
        virtual bool IsFloat()  { return false; }
        virtual bool IsInt()    { return false; }
        virtual bool IsChar()   { return false; }

        virtual int GetSize() { return m_nodeSize; }
        virtual void SetSize(int size) { m_nodeSize = size; }

        virtual int GetOffset() { return m_nodeOffset; }
        virtual void SetOffset(int offset) { m_nodeOffset = offset; }

        virtual string AttributesToString()
        {
            if (m_nodeSize == 0 && m_nodeOffset == 0) return string("");

            return " size=\"" + std::to_string(m_nodeSize) +
                   "\" offset=\"" + std::to_string(m_nodeOffset) + "\"";
        }

        virtual cDeclNode *GetType() = 0;
        virtual cDeclNode *GetDecl() { return this; }
        virtual string GetName() = 0;

        virtual bool IsCompatibleWith(cDeclNode *type)
        {
            if (type == nullptr) return false;
            if (this == type) return true;

            auto rank = [](cDeclNode *t) -> int
            {
                if (t == nullptr) return -1;
                if (t->IsChar()) return 1;
                if (t->IsInt() && !t->IsFloat() && t->GetSize() == 4) return 2;
                if (t->IsInt() && !t->IsFloat() && t->GetSize() == 8) return 3;
                if (t->IsFloat() && t->GetSize() == 4) return 4;
                if (t->IsFloat() && t->GetSize() == 8) return 5;
                return -1;
            };

            int dstRank = rank(this);
            int srcRank = rank(type);

            if (dstRank > 0 && srcRank > 0)
            {
                return srcRank <= dstRank;
            }

            return false;
        }

    protected:
        int m_nodeSize;
        int m_nodeOffset;
};
