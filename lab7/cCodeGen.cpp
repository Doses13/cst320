#include <iostream>
#include "cCodeGen.h"
#include "astnodes.h"
#include "emit.h"

using std::string;

cCodeGen::cCodeGen(const string &filename)
    : cVisitor(), m_filename(filename), m_currentFunction(""), m_returnLabel("")
{
    if (!InitOutput(filename))
    {
        std::cerr << "ERROR: could not open output file " << filename << "\n";
    }
}

cCodeGen::~cCodeGen()
{
    FinalizeOutput();
}

void cCodeGen::Emit(const string &text)
{
    EmitString(text);
}

void cCodeGen::EmitLine(const string &text)
{
    EmitString(text);
    EmitString("\n");
}

void cCodeGen::EmitLabel(const string &label)
{
    EmitString(label);
    EmitString(":\n");
}

string cCodeGen::NewLabel()
{
    return GenerateLabel();
}

void cCodeGen::Visit(cProgramNode *node)
{
    if (node == nullptr) return;

    EmitLine(".function main");
    EmitLabel("main");

    if (node->GetSize() > 0)
    {
        EmitLine("ADJSP " + std::to_string(node->GetSize()));
    }

    cBlockNode *block = node->GetBlock();
    if (block != nullptr) block->Visit(this);

    EmitLine("PUSH 0");
    EmitLine("RETURNV");
}

void cCodeGen::Visit(cBlockNode *node)
{
    if (node != nullptr) node->VisitAllChildren(this);
}

void cCodeGen::Visit(cStmtsNode *node)
{
    if (node != nullptr) node->VisitAllChildren(this);
}

void cCodeGen::Visit(cFuncDeclNode *node)
{
    if (node == nullptr) return;

    m_currentFunction = node->GetName();
    m_returnLabel = NewLabel();

    EmitLabel(m_currentFunction);   // this is what will create "main:"

    cDeclsNode *decls = node->GetDecls();
    cStmtsNode *stmts = node->GetStmts();

    if (decls != nullptr) decls->Visit(this);
    if (stmts != nullptr) stmts->Visit(this);

    EmitLabel(m_returnLabel);

    // temporary function-exit code goes here
}

void cCodeGen::Visit(cReturnNode *node)
{
    if (node != nullptr) node->VisitAllChildren(this);
}

void cCodeGen::Visit(cPrintNode *node)
{
    if (node == nullptr) return;

    cExprNode *expr = node->GetExpr();
    if (expr != nullptr) expr->Visit(this);

    EmitLine("CALL @print");
    EmitLine("POP");
}

void cCodeGen::Visit(cPrintsNode *node)
{
    if (node != nullptr) node->VisitAllChildren(this);
}

void cCodeGen::Visit(cAssignNode *node)
{
    if (node == nullptr) return;

    cExprNode *rhs = node->GetRHS();
    cVarExprNode *lhs = dynamic_cast<cVarExprNode*>(node->GetLHS());
    if (rhs == nullptr || lhs == nullptr) return;

    rhs->Visit(this);                       // leaves value on stack
    EmitLine("DUP");
    EmitLine("PUSH " + std::to_string(lhs->GetOffset()));
    EmitLine("PUSHFP");
    EmitLine("PLUS");
    EmitLine("POPVARIND");
    EmitLine("POP");
}

void cCodeGen::Visit(cIfNode *node)
{
    if (node != nullptr) node->VisitAllChildren(this);
}

void cCodeGen::Visit(cWhileNode *node)
{
    if (node != nullptr) node->VisitAllChildren(this);
}

void cCodeGen::Visit(cFuncCallNode *node)
{
    if (node != nullptr) node->VisitAllChildren(this);
}

void cCodeGen::Visit(cParamsNode *node)
{
    if (node != nullptr) node->VisitAllChildren(this);
}

void cCodeGen::Visit(cVarExprNode *node)
{
    if (node == nullptr) return;

    EmitLine("PUSH " + std::to_string(node->GetOffset()));
    EmitLine("PUSHFP");
    EmitLine("PLUS");
    EmitLine("PUSHVARIND");
}

void cCodeGen::Visit(cBinaryExprNode *node)
{
    if (node == nullptr) return;

    cExprNode *left = node->GetLeft();
    cExprNode *right = node->GetRight();
    cOpNode *op = node->GetOp();
    if (left == nullptr || right == nullptr || op == nullptr) return;

    left->Visit(this);
    right->Visit(this);

    switch (op->GetOp())
    {
        case '+': EmitLine("PLUS"); break;
        case '-': EmitLine("MINUS"); break;
        case '*': EmitLine("TIMES"); break;      // if this doesn't assemble, try MULTIPLY
        case '/': EmitLine("DIVIDE"); break;
        case '%': EmitLine("MOD"); break;

        case '<': EmitLine("LT"); break;
        case '>': EmitLine("GT"); break;
        case GE:  EmitLine("GE"); break;
        case LE:  EmitLine("LE"); break;
        case EQUALS:     EmitLine("EQ"); break;
        case NOT_EQUALS: EmitLine("NE"); break;
        case AND: EmitLine("AND"); break;
        case OR:  EmitLine("OR"); break;

        default:
            std::cerr << "Unknown binary op " << op->GetOp() << "\n";
            break;
    }
}

void cCodeGen::Visit(cIntExprNode *node)
{
    if (node == nullptr) return;

    EmitLine("PUSH " + std::to_string(node->GetValue()));
}

void cCodeGen::Visit(cFloatExprNode *node)
{
    if (node != nullptr) node->VisitAllChildren(this);
}