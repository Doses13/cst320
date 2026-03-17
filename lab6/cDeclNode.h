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

            // Non-scalar types only match exactly.
            if (IsStruct() || type->IsStruct() ||
                IsArray()  || type->IsArray()  ||
                IsFunc()   || type->IsFunc())
            {
                return false;
            }

            auto isInt32 = [](cDeclNode *t) -> bool
            {
                return (t != nullptr && t->IsInt() && !t->IsFloat() && !t->IsChar() && t->GetSize() == 4);
            };

            auto isInt64 = [](cDeclNode *t) -> bool
            {
                return (t != nullptr && t->IsInt() && !t->IsFloat() && !t->IsChar() && t->GetSize() == 8);
            };

            auto isFloat32 = [](cDeclNode *t) -> bool
            {
                return (t != nullptr && t->IsFloat() && t->GetSize() == 4);
            };

            auto isFloat64 = [](cDeclNode *t) -> bool
            {
                return (t != nullptr && t->IsFloat() && t->GetSize() == 8);
            };

            // char only accepts char directly. cSemantics special-cases
            // assignable constant/range-preserving int expressions.
            if (IsChar())
            {
                return type->IsChar();
            }

            if (isInt32(this))
            {
                return type->IsChar() || isInt32(type);
            }

            if (isInt64(this))
            {
                return type->IsChar() || isInt32(type) || isInt64(type);
            }

            if (isFloat32(this))
            {
                return type->IsChar() || isInt32(type) || isFloat32(type);
            }

            if (isFloat64(this))
            {
                return type->IsChar() || isInt32(type) || isInt64(type) ||
                       isFloat32(type) || isFloat64(type);
            }

            return false;
        }

    protected:
        int m_nodeSize;
        int m_nodeOffset;
};
