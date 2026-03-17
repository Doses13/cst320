#pragma once
//**************************************
// cSizeOffset.h
//
// Visitor for Lab 6 size/offset computation.
//

#include <string>
#include "cVisitor.h"

class cProgramNode;
class cBlockNode;
class cDeclsNode;
class cVarDeclNode;
class cArrayDeclNode;
class cStructDeclNode;
class cFuncDeclNode;
class cArgsNode;
class cParamsNode;
class cVarExprNode;
class cExprNode;
class cDeclNode;

class cSizeOffset : public cVisitor
{
    public:
        cSizeOffset();

        virtual void Visit(cProgramNode *node) override;
        virtual void Visit(cBlockNode *node) override;
        virtual void Visit(cDeclsNode *node) override;
        virtual void Visit(cVarDeclNode *node) override;
        virtual void Visit(cArrayDeclNode *node) override;
        virtual void Visit(cStructDeclNode *node) override;
        virtual void Visit(cFuncDeclNode *node) override;
        virtual void Visit(cArgsNode *node) override;
        virtual void Visit(cParamsNode *node) override;
        virtual void Visit(cVarExprNode *node) override;
        virtual void Visit(cBinaryExprNode *node) override;
        virtual void Visit(cFuncCallNode *node) override;

    protected:
        static const int WORD_SIZE = 4;

        int m_nextOffset;
        int m_highWater;
        int m_paramOffset;
        bool m_layoutArgs;
        bool m_inStructLayout;

        int AlignToWord(int value) const;
        int LocalAlignOffset(int offset, int size) const;
        int ParamSlotSize(int size) const;
        int FinalAlignedSize(int size) const;
        void AllocateLocal(cDeclNode *decl);
        cDeclNode *FindFieldDecl(cDeclNode *type, const std::string &name);
        int GetNodeSize(cAstNode *node);
};
