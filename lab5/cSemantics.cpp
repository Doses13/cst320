#include "astnodes.h"
#include "cSemantics.h"

void cSemantics::Visit(cAssignNode *node)
{
    cExprNode *lhs = node->GetLHS();
    cExprNode *rhs = node->GetRHS();

    if (lhs != nullptr) lhs->Visit(this);
    if (rhs != nullptr) rhs->Visit(this);

    if (lhs == nullptr || rhs == nullptr) return;

    cDeclNode *lhsType = lhs->GetType();
    cDeclNode *rhsType = rhs->GetType();

    if (lhsType == nullptr || rhsType == nullptr) return;

    if (!lhsType->IsCompatibleWith(rhsType))
    {
        node->SemanticError("Cannot assign " + rhsType->GetName() +
                            " to " + lhsType->GetName());
    }
}

void cSemantics::Visit(cVarExprNode *node)
{
    node->VisitAllChildren(this);

    cDeclNode *decl = node->GetDecl();
    if (decl == nullptr) return;

    cSymbol *sym = node->GetSymbol();
    string name = (sym == nullptr) ? string("") : sym->GetName();

    if (decl->IsFunc())
    {
        node->SemanticError("Symbol " + name + " is a function, not a variable");
    }

    int partCount = node->GetPartCount();
    if (partCount == 0) return;

    // For an indexed varref like bb[5], the thing that must be an array is the
    // VARIABLE'S TYPE, not the declaration node itself. A variable declared as
    // "my_array bb;" has a cVarDeclNode declaration, but its type is the array.
    cDeclNode *type = decl->GetType();

    for (int i = 0; i < partCount; i++)
    {
        if (type == nullptr || !type->IsArray())
        {
            node->SemanticError(name + " is not an array");
            return;
        }

        cExprNode *index = node->GetPart(i);
        if (index != nullptr)
        {
            cDeclNode *indexType = index->GetType();
            if (indexType != nullptr && !indexType->IsInt())
            {
                node->SemanticError("Index of " + name + " is not an int");
            }
        }

        type = type->GetType();
    }
}

void cSemantics::Visit(cFuncCallNode *node)
{
    node->VisitAllChildren(this);

    cDeclNode *decl = node->GetDecl();
    if (decl == nullptr) return;
    if (!decl->IsFunc()) return;

    cFuncDeclNode *funcDecl = dynamic_cast<cFuncDeclNode*>(decl);
    if (funcDecl == nullptr) return;

    string name = node->GetName();

    cParamsNode *actuals = node->GetParams();
    int actualCount = (actuals == nullptr) ? 0 : actuals->GetCount();
    int formalCount = funcDecl->GetParamCount();

    if (actualCount != formalCount)
    {
        node->SemanticError(name + " called with wrong number of arguments");
        return;
    }

    for (int i = 0; i < actualCount; i++)
    {
        cExprNode *actual = actuals->GetParam(i);
        cDeclNode *formalDecl = funcDecl->GetParamDecl(i);
        if (actual == nullptr || formalDecl == nullptr) continue;

        cDeclNode *formalType = formalDecl->GetType();
        cDeclNode *actualType = actual->GetType();
        if (formalType == nullptr || actualType == nullptr) continue;

        if (!formalType->IsCompatibleWith(actualType))
        {
            node->SemanticError("function " + name + " called with incompatible argument");
            break;
        }
    }

    if (!funcDecl->HasDefinition())
    {
        node->SemanticError("Function " + name + " not fully defined");
    }
}
