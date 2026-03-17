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

void cCodeGen::EmitVarAddress(cVarExprNode *node)
{
    if (node == nullptr) return;

    // start with FP + base offset
    EmitLine("PUSH " + std::to_string(node->GetOffset()));
    EmitLine("PUSHFP");
    EmitLine("PLUS");

    const std::vector<int> &rowSizes = node->GetRowSizes();
    int row = 0;

    for (int i = 0; i < node->GetPartCount(); i++)
    {
        cExprNode *indexExpr = node->GetPartExpr(i);
        if (indexExpr == nullptr) continue;

        indexExpr->Visit(this);   // push index value

        int rowSize = 0;
        if (row < (int)rowSizes.size()) rowSize = rowSizes[row++];
        EmitLine("PUSH " + std::to_string(rowSize));
        EmitLine("TIMES");        // if TIMES fails, use MULTIPLY
        EmitLine("PLUS");         // add displacement to base address
    }
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

    std::string skipLabel = NewLabel();
    std::string savedFunction = m_currentFunction;
    std::string savedReturn = m_returnLabel;

    EmitLine("JUMP @" + skipLabel);

    m_currentFunction = node->GetName();
    m_returnLabel = NewLabel();

    EmitLabel(m_currentFunction);

    if (node->GetSize() > 0)
    {
        EmitLine("ADJSP " + std::to_string(node->GetSize()));
    }

    cDeclsNode *decls = node->GetDecls();
    cStmtsNode *stmts = node->GetStmts();

    if (decls != nullptr) decls->Visit(this);
    if (stmts != nullptr) stmts->Visit(this);

    EmitLine("PUSH 0");          // default fallthrough return
    EmitLabel(m_returnLabel);
    EmitLine("RETURNV");

    EmitLabel(skipLabel);

    m_currentFunction = savedFunction;
    m_returnLabel = savedReturn;
}

void cCodeGen::Visit(cReturnNode *node)
{
    if (node == nullptr) return;

    cExprNode *expr = node->GetExpr();
    if (expr != nullptr)
        expr->Visit(this);
    else
        EmitLine("PUSH 0");

    EmitLine("JUMP @" + m_returnLabel);
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

    rhs->Visit(this);        // value
    EmitLine("DUP");         // preserve assignment result
    EmitVarAddress(lhs);     // address

    if (lhs->GetSize() == 1)
        EmitLine("POPCVARIND");
    else
        EmitLine("POPVARIND");

    EmitLine("POP");
}

void cCodeGen::Visit(cIfNode *node)
{
    if (node == nullptr) return;

    std::string falseLabel = NewLabel();
    std::string endLabel = NewLabel();

    cExprNode *cond = node->GetCond();
    cStmtsNode *thenPart = node->GetThenPart();
    cStmtsNode *elsePart = node->GetElsePart();

    if (cond != nullptr) cond->Visit(this);

    EmitLine("JUMPE @" + falseLabel);

    if (thenPart != nullptr) thenPart->Visit(this);

    if (elsePart != nullptr)
    {
        EmitLine("JUMP @" + endLabel);
        EmitLabel(falseLabel);
        elsePart->Visit(this);
        EmitLabel(endLabel);
    }
    else
    {
        EmitLabel(falseLabel);
    }
}

void cCodeGen::Visit(cWhileNode *node)
{
    if (node == nullptr) return;

    std::string topLabel = NewLabel();
    std::string endLabel = NewLabel();

    cExprNode *cond = node->GetCond();
    cStmtNode *stmt = node->GetStmt();

    EmitLabel(topLabel);

    if (cond != nullptr) cond->Visit(this);

    EmitLine("JUMPE @" + endLabel);

    if (stmt != nullptr) stmt->Visit(this);

    EmitLine("JUMP @" + topLabel);
    EmitLabel(endLabel);
}

void cCodeGen::Visit(cFuncCallNode *node)
{
    if (node == nullptr) return;

    cParamsNode *params = node->GetParams();
    if (params != nullptr)
    {
        for (int i = params->GetCount() - 1; i >= 0; --i)
        {
            cExprNode *arg = params->GetParam(i);
            if (arg != nullptr) arg->Visit(this);
        }
    }

    EmitLine("CALL @" + node->GetName());

    int paramBytes = (params == nullptr) ? 0 : params->GetSize();
    int wordCount = paramBytes / 4;

    // keep return value, discard argument words underneath it
    for (int i = 0; i < wordCount; ++i)
    {
        EmitLine("SWAP");
        EmitLine("POP");
    }
}

void cCodeGen::Visit(cParamsNode *node)
{
    if (node != nullptr) node->VisitAllChildren(this);
}

void cCodeGen::Visit(cVarExprNode *node)
{
    if (node == nullptr) return;

    EmitVarAddress(node);

    if (node->GetSize() == 1)
        EmitLine("PUSHCVARIND");
    else
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