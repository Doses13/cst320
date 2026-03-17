#include <algorithm>
#include "astnodes.h"
#include "cSizeOffset.h"

cSizeOffset::cSizeOffset()
    : cVisitor(), m_nextOffset(0), m_highWater(0), m_paramOffset(-8),
      m_layoutArgs(false), m_inStructLayout(false)
{
}

int cSizeOffset::AlignToWord(int value) const
{
    return ((value + WORD_SIZE - 1) / WORD_SIZE) * WORD_SIZE;
}

int cSizeOffset::LocalAlignOffset(int offset, int size) const
{
    return (size > 1) ? AlignToWord(offset) : offset;
}

int cSizeOffset::ParamSlotSize(int size) const
{
    if (size <= 1) return WORD_SIZE;
    return AlignToWord(size);
}

int cSizeOffset::FinalAlignedSize(int size) const
{
    if (size == 0) return 0;
    return AlignToWord(size);
}

void cSizeOffset::AllocateLocal(cDeclNode *decl)
{
    if (decl == nullptr) return;

    int size = decl->GetSize();
    int offset = LocalAlignOffset(m_nextOffset, size);
    decl->SetOffset(offset);

    m_nextOffset = offset + size;
    m_highWater = std::max(m_highWater, m_nextOffset);
}

cDeclNode *cSizeOffset::FindFieldDecl(cDeclNode *type, const std::string &name)
{
    if (type == nullptr || !type->IsStruct()) return nullptr;

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

int cSizeOffset::GetNodeSize(cAstNode *node)
{
    if (node == nullptr) return 0;

    cVarExprNode *varRef = dynamic_cast<cVarExprNode*>(node);
    if (varRef != nullptr) return varRef->GetSize();

    cDeclNode *decl = dynamic_cast<cDeclNode*>(node);
    if (decl != nullptr) return decl->GetSize();

    cExprNode *expr = dynamic_cast<cExprNode*>(node);
    if (expr != nullptr)
    {
        cDeclNode *type = expr->GetType();
        return (type == nullptr) ? 0 : type->GetSize();
    }

    return 0;
}

void cSizeOffset::Visit(cProgramNode *node)
{
    int savedNext = m_nextOffset;
    int savedHigh = m_highWater;

    m_nextOffset = 0;
    m_highWater = 0;

    cBlockNode *block = node->GetBlock();
    if (block != nullptr) block->Visit(this);

    // Program size is the final high-water mark.
    node->SetSize(m_highWater);

    m_nextOffset = savedNext;
    m_highWater = savedHigh;
}

void cSizeOffset::Visit(cBlockNode *node)
{
    int savedNext = m_nextOffset;
    int savedHigh = m_highWater;
    int baseOffset = savedNext;

    // This block reuses enclosing storage and grows from the current base.
    m_highWater = baseOffset;

    cDeclsNode *decls = node->GetDecls();
    cStmtsNode *stmts = node->GetStmts();

    if (decls != nullptr) decls->Visit(this);
    if (stmts != nullptr) stmts->Visit(this);

    int blockHigh = m_highWater;

    // Block size is raw high-water mark relative to its base.
    node->SetSize(blockHigh - baseOffset);

    // Reclaim inner-scope space, but preserve the high-water mark reached.
    m_nextOffset = savedNext;
    m_highWater = std::max(savedHigh, blockHigh);
}

void cSizeOffset::Visit(cDeclsNode *node)
{
    int baseOffset = m_nextOffset;

    for (int i = 0; i < node->GetCount(); i++)
    {
        cDeclNode *decl = node->GetDecl(i);
        if (decl == nullptr) continue;

        decl->Visit(this);

        if (decl->IsVar())
        {
            AllocateLocal(decl);
        }
    }

    // Decls size is also the raw high-water span from the start of this list.
    node->SetSize(m_nextOffset - baseOffset);
}

void cSizeOffset::Visit(cVarDeclNode *node)
{
    cDeclNode *type = node->GetType();
    node->SetSize((type == nullptr) ? 0 : type->GetSize());
}

void cSizeOffset::Visit(cArrayDeclNode *node)
{
    cDeclNode *type = node->GetType();
    int elemSize = (type == nullptr) ? 0 : type->GetSize();
    node->SetSize(elemSize * node->GetCount());
}

void cSizeOffset::Visit(cStructDeclNode *node)
{
    int savedNext = m_nextOffset;
    int savedHigh = m_highWater;
    bool savedStructLayout = m_inStructLayout;

    m_nextOffset = 0;
    m_highWater = 0;
    m_inStructLayout = true;

    cDeclsNode *fields = node->GetFields();
    if (fields != nullptr) fields->Visit(this);

    node->SetSize(FinalAlignedSize(m_highWater));
    node->SetOffset(0);

    m_inStructLayout = savedStructLayout;
    m_nextOffset = savedNext;
    m_highWater = savedHigh;
}

void cSizeOffset::Visit(cFuncDeclNode *node)
{
    int savedNext = m_nextOffset;
    int savedHigh = m_highWater;
    int savedParamOffset = m_paramOffset;
    bool savedLayoutArgs = m_layoutArgs;

    m_paramOffset = -8;
    m_layoutArgs = true;
    cArgsNode *args = node->GetArgs();
    if (args != nullptr) args->Visit(this);
    m_layoutArgs = false;

    m_nextOffset = 0;
    m_highWater = 0;

    cDeclsNode *decls = node->GetDecls();
    cStmtsNode *stmts = node->GetStmts();
    if (decls != nullptr) decls->Visit(this);
    if (stmts != nullptr) stmts->Visit(this);

    // Function frame size should reflect the raw high-water mark.
    node->SetSize(m_highWater);
    node->SetOffset(0);

    m_nextOffset = savedNext;
    m_highWater = savedHigh;
    m_paramOffset = savedParamOffset;
    m_layoutArgs = savedLayoutArgs;
}

void cSizeOffset::Visit(cArgsNode *node)
{
    int totalSize = 0;

    for (int i = 0; i < node->GetCount(); i++)
    {
        cDeclNode *arg = node->GetArg(i);
        if (arg == nullptr) continue;

        arg->Visit(this);

        int slotSize = ParamSlotSize(arg->GetSize());
        m_paramOffset -= slotSize;
        arg->SetOffset(m_paramOffset);
        totalSize += slotSize;
    }

    node->SetSize(totalSize);
}

void cSizeOffset::Visit(cParamsNode *node)
{
    int totalSize = 0;

    for (int i = 0; i < node->GetCount(); i++)
    {
        cExprNode *param = node->GetParam(i);
        if (param == nullptr) continue;

        param->Visit(this);
        totalSize += ParamSlotSize(GetNodeSize(param));
    }

    node->SetSize(totalSize);
}

void cSizeOffset::Visit(cVarExprNode *node)
{
    // Only recurse into expression parts such as array indices.
    // Do not blindly visit the leading symbol child.
    for (int i = 0; i < node->GetPartCount(); i++)
    {
        cAstNode *part = node->GetPartNode(i);
        cExprNode *indexExpr = dynamic_cast<cExprNode*>(part);
        if (indexExpr != nullptr)
        {
            indexExpr->Visit(this);
        }
    }

    cDeclNode *decl = node->GetDecl();
    if (decl == nullptr)
    {
        node->SetSize(0);
        node->SetOffset(0);
        node->ClearRowSizes();
        return;
    }

    int size = decl->GetSize();
    int offset = decl->GetOffset();
    cDeclNode *currentType = decl->GetType();

    node->ClearRowSizes();

    for (int i = 0; i < node->GetPartCount(); i++)
    {
        cAstNode *part = node->GetPartNode(i);

        cSymbol *fieldSym = dynamic_cast<cSymbol*>(part);
        if (fieldSym != nullptr)
        {
            cDeclNode *fieldDecl = FindFieldDecl(currentType, fieldSym->GetName());
            if (fieldDecl != nullptr)
            {
                offset += fieldDecl->GetOffset();
                size = fieldDecl->GetSize();
                currentType = fieldDecl->GetType();
            }
            continue;
        }

        cExprNode *indexExpr = dynamic_cast<cExprNode*>(part);
        if (indexExpr != nullptr)
        {
            if (currentType != nullptr && currentType->IsArray())
            {
                cDeclNode *elemType = currentType->GetType();
                node->AddRowSize((elemType == nullptr) ? 0 : elemType->GetSize());
                currentType = elemType;
            }
            continue;
        }
    }

    node->SetSize(size);
    node->SetOffset(offset);
}

void cSizeOffset::Visit(cBinaryExprNode *node)
{
    if (node == nullptr) return;

    cExprNode *left = node->GetLeft();
    cOpNode *op = node->GetOp();
    cExprNode *right = node->GetRight();

    if (left != nullptr) left->Visit(this);
    if (op != nullptr) op->Visit(this);
    if (right != nullptr) right->Visit(this);
}

void cSizeOffset::Visit(cFuncCallNode *node)
{
    if (node == nullptr) return;

    cParamsNode *params = node->GetParams();
    if (params != nullptr) params->Visit(this);
}