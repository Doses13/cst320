#pragma once
//**************************************
// cVarExprNode.h
//
// AST node for a variable reference
//

#include <vector>
#include "cExprNode.h"
#include "cSymbol.h"
#include "cDeclNode.h"
#include "cStructDeclNode.h"

class cVarExprNode : public cExprNode
{
    public:
        cVarExprNode(cSymbol *sym) : cExprNode(), m_size(0), m_offset(0)
        {
            AddChild(sym);

            if (sym == nullptr || sym->GetDecl() == nullptr)
            {
                string name = (sym == nullptr) ? string("<null>") : sym->GetName();
                SemanticParseError("Symbol " + name + " not defined");
            }
        }

        void AddPart(cAstNode *node)
        {
            AddChild(node);
        }

        virtual string NodeType() { return string("varref"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        virtual string AttributesToString()
        {
            if (m_size == 0 && m_offset == 0 && m_rowSizes.empty()) return string("");

            string result = " size=\"" + std::to_string(m_size) +
                            "\" offset=\"" + std::to_string(m_offset) + "\"";

            if (!m_rowSizes.empty())
            {
                result += " rowsizes=\"";
                for (size_t i = 0; i < m_rowSizes.size(); i++)
                {
                    if (i > 0) result += " ";
                    result += std::to_string(m_rowSizes[i]);
                }
                result += "\"";
            }

            return result;
        }

        cSymbol *GetSymbol()
        {
            return static_cast<cSymbol*>(GetChild(0));
        }

        string GetName()
        {
            cSymbol *sym = GetSymbol();
            return (sym == nullptr) ? string("") : sym->GetName();
        }

        int GetPartCount()
        {
            int children = NumChildren();
            return (children <= 1) ? 0 : (children - 1);
        }

        cAstNode *GetPartNode(int index)
        {
            return GetChild(index + 1);
        }

        cExprNode *GetPart(int index)
        {
            return dynamic_cast<cExprNode*>(GetChild(index + 1));
        }

        cExprNode *GetPartExpr(int index)
        {
            return dynamic_cast<cExprNode*>(GetChild(index + 1));
        }

        cSymbol *GetPartSymbol(int index)
        {
            return dynamic_cast<cSymbol*>(GetChild(index + 1));
        }

        int GetSize()
        {
            return m_size;
        }

        void SetSize(int size)
        {
            m_size = size;
        }

        int GetOffset()
        {
            return m_offset;
        }

        void SetOffset(int offset)
        {
            m_offset = offset;
        }

        void ClearRowSizes()
        {
            m_rowSizes.clear();
        }

        void AddRowSize(int size)
        {
            m_rowSizes.push_back(size);
        }

        const std::vector<int> &GetRowSizes() const
        {
            return m_rowSizes;
        }

        virtual cDeclNode *GetDecl()
        {
            cSymbol *sym = GetSymbol();
            return (sym == nullptr) ? nullptr : sym->GetDecl();
        }

        virtual cDeclNode *GetType()
        {
            cDeclNode *decl = GetDecl();
            if (decl == nullptr) return nullptr;

            cDeclNode *type = decl->GetType();
            if (type == nullptr) return nullptr;

            for (int i = 0; i < GetPartCount(); i++)
            {
                cSymbol *fieldSym = GetPartSymbol(i);
                if (fieldSym != nullptr)
                {
                    cDeclNode *fieldDecl = FindFieldDecl(type, fieldSym->GetName());
                    if (fieldDecl == nullptr) return nullptr;
                    type = fieldDecl->GetType();
                    if (type == nullptr) return nullptr;
                    continue;
                }

                cExprNode *indexExpr = GetPartExpr(i);
                if (indexExpr != nullptr)
                {
                    if (!type->IsArray()) return nullptr;
                    type = type->GetType();
                    if (type == nullptr) return nullptr;
                    continue;
                }

                return nullptr;
            }

            return type;
        }

    protected:
        cDeclNode *FindFieldDecl(cDeclNode *type, const string &name)
        {
            cStructDeclNode *structType = dynamic_cast<cStructDeclNode*>(type);
            if (structType == nullptr) return nullptr;

            cDeclsNode *fields = structType->GetFields();
            if (fields == nullptr) return nullptr;

            for (int i = 0; i < fields->GetCount(); i++)
            {
                cDeclNode *field = fields->GetDecl(i);
                if (field != nullptr && field->GetName() == name)
                {
                    return field;
                }
            }

            return nullptr;
        }

        int m_size;
        int m_offset;
        std::vector<int> m_rowSizes;
};
