#pragma once
//**************************************
// cCodeGen.h
//
// Code generation visitor for Lab 7.
//

#include <string>
#include "cVisitor.h"

class cAstNode;
class cProgramNode;
class cBlockNode;
class cStmtsNode;
class cStmtNode;
class cDeclNode;
class cExprNode;
class cIntExprNode;
class cFloatExprNode;
class cBinaryExprNode;
class cOpNode;
class cPrintNode;
class cPrintsNode;
class cVarExprNode;
class cAssignNode;
class cIfNode;
class cWhileNode;
class cReturnNode;
class cFuncDeclNode;
class cFuncCallNode;
class cParamsNode;

class cCodeGen : public cVisitor
{
    public:
        explicit cCodeGen(const std::string &filename);
        virtual ~cCodeGen();

        virtual void Visit(cProgramNode *node) override;
        virtual void Visit(cBlockNode *node) override;
        virtual void Visit(cStmtsNode *node) override;

        virtual void Visit(cFuncDeclNode *node) override;
        virtual void Visit(cReturnNode *node) override;

        virtual void Visit(cPrintNode *node) override;
        virtual void Visit(cPrintsNode *node) override;

        virtual void Visit(cAssignNode *node) override;
        virtual void Visit(cIfNode *node) override;
        virtual void Visit(cWhileNode *node) override;

        virtual void Visit(cFuncCallNode *node) override;
        virtual void Visit(cParamsNode *node) override;

        virtual void Visit(cVarExprNode *node) override;
        virtual void Visit(cBinaryExprNode *node) override;
        virtual void Visit(cIntExprNode *node) override;
        virtual void Visit(cFloatExprNode *node) override;

    protected:
        std::string m_filename;
        std::string m_currentFunction;
        std::string m_returnLabel;

        void Emit(const std::string &text);
        void EmitLine(const std::string &text);
        void EmitLabel(const std::string &label);
        std::string NewLabel();

        void GenExpr(cExprNode *node);
        void GenLVal(cVarExprNode *node);
        void GenRVal(cVarExprNode *node);

        void GenBinaryOp(cBinaryExprNode *node);
        void GenCompareOp(cBinaryExprNode *node);
        void GenFuncCall(cFuncCallNode *node);

        int GetNodeSize(cAstNode *node) const;
        bool IsByteNode(cAstNode *node) const;
};