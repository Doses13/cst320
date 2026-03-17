#pragma once
//**************************************************************
// Definition of the virtual Visitor class.
//

class cAstNode;
class cBlockNode;
class cDeclNode;
class cDeclsNode;
class cExprNode;
class cIntExprNode;
class cFloatExprNode;
class cBinaryExprNode;
class cOpNode;
class cPrintNode;
class cPrintsNode;
class cProgramNode;
class cStmtNode;
class cStmtsNode;
class cSymbol;
class cVarDeclNode;
class cVarExprNode;
class cArrayDeclNode;
class cStructDeclNode;
class cIfNode;
class cWhileNode;
class cReturnNode;
class cAssignNode;
class cArgsNode;
class cParamsNode;
class cFuncDeclNode;
class cFuncCallNode;

class cVisitor
{
    public:
        virtual void VisitAllNodes(cAstNode *node);

        virtual void Visit(cAstNode *node);
        virtual void Visit(cBlockNode *node);
        virtual void Visit(cDeclNode *node);
        virtual void Visit(cDeclsNode *node);
        virtual void Visit(cExprNode *node);
        virtual void Visit(cIntExprNode *node);
        virtual void Visit(cFloatExprNode *node);
        virtual void Visit(cBinaryExprNode *node);
        virtual void Visit(cOpNode *node);
        virtual void Visit(cPrintNode *node);
        virtual void Visit(cPrintsNode *node);
        virtual void Visit(cProgramNode *node);
        virtual void Visit(cStmtNode *node);
        virtual void Visit(cStmtsNode *node);
        virtual void Visit(cSymbol *node);
        virtual void Visit(cVarDeclNode *node);
        virtual void Visit(cVarExprNode *node);
        virtual void Visit(cArrayDeclNode *node);
        virtual void Visit(cStructDeclNode *node);
        virtual void Visit(cIfNode *node);
        virtual void Visit(cWhileNode *node);
        virtual void Visit(cReturnNode *node);
        virtual void Visit(cAssignNode *node);
        virtual void Visit(cArgsNode *node);
        virtual void Visit(cParamsNode *node);
        virtual void Visit(cFuncDeclNode *node);
        virtual void Visit(cFuncCallNode *node);
};
