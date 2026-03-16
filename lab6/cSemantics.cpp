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
    cDeclNode *decl = node->GetDecl();
    if (decl == nullptr) return;

    cSymbol *sym = node->GetSymbol();
    string name = (sym == nullptr) ? string("") : sym->GetName();

    if (decl->IsFunc())
    {
        node->SemanticError("Symbol " + name + " is a function, not a variable");
    }

    cDeclNode *type = decl->GetType();
    int partCount = node->GetPartCount();

    for (int i = 0; i < partCount; i++)
    {
        cSymbol *fieldSym = node->GetPartSymbol(i);
        if (fieldSym != nullptr)
        {
            cStructDeclNode *structType = dynamic_cast<cStructDeclNode*>(type);
            if (structType == nullptr)
            {
                node->SemanticError(name + " is not a struct");
                return;
            }

            cDeclsNode *fields = structType->GetFields();
            cDeclNode *fieldDecl = nullptr;

            if (fields != nullptr)
            {
                for (int j = 0; j < fields->GetCount(); j++)
                {
                    cDeclNode *field = fields->GetDecl(j);
                    if (field != nullptr && field->GetName() == fieldSym->GetName())
                    {
                        fieldDecl = field;
                        break;
                    }
                }
            }

            if (fieldDecl == nullptr)
            {
                node->SemanticError("Struct " + structType->GetName() +
                                    " has no field named " + fieldSym->GetName());
                return;
            }

            type = fieldDecl->GetType();
            continue;
        }

        cExprNode *index = node->GetPartExpr(i);
        if (index != nullptr)
        {
            index->Visit(this);

            if (type == nullptr || !type->IsArray())
            {
                node->SemanticError(name + " is not an array");
                return;
            }

            cDeclNode *indexType = index->GetType();
            if (indexType != nullptr && !indexType->IsInt())
            {
                node->SemanticError("Index of " + name + " is not an int");
            }

            type = type->GetType();
            continue;
        }

        node->SemanticError("Invalid variable reference");
        return;
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
