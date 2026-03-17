#include <cstdlib>
#include <limits>
#include <string>

#include "astnodes.h"
#include "cSemantics.h"
#include "cSymbolTable.h"

using std::string;

namespace
{
    cDeclNode *FindFieldDecl(cDeclNode *type, const string &fieldName)
    {
        cStructDeclNode *structType = dynamic_cast<cStructDeclNode*>(type);
        if (structType == nullptr) return nullptr;

        cDeclsNode *fields = structType->GetFields();
        if (fields == nullptr) return nullptr;

        for (int i = 0; i < fields->GetCount(); i++)
        {
            cDeclNode *field = fields->GetDecl(i);
            if (field != nullptr && field->GetName() == fieldName)
            {
                return field;
            }
        }

        return nullptr;
    }

    bool ExtractNumericValue(const string &xml, string &value)
    {
        size_t pos = xml.find("value=\"");
        if (pos == string::npos) return false;
        pos += 7;
        size_t end = xml.find('"', pos);
        if (end == string::npos) return false;
        value = xml.substr(pos, end - pos);
        return true;
    }

    bool TryGetIntLiteral(cExprNode *expr, long long &value)
    {
        cIntExprNode *intNode = dynamic_cast<cIntExprNode*>(expr);
        if (intNode == nullptr) return false;

        string text;
        if (!ExtractNumericValue(intNode->ToString(), text)) return false;

        char *end = nullptr;
        value = std::strtoll(text.c_str(), &end, 10);
        return (end != nullptr && *end == '\0');
    }

    bool TryEvalConstInt(cExprNode *expr, long long &value)
    {
        if (TryGetIntLiteral(expr, value)) return true;

        cBinaryExprNode *bin = dynamic_cast<cBinaryExprNode*>(expr);
        if (bin == nullptr) return false;

        long long left = 0;
        long long right = 0;
        if (!TryEvalConstInt(bin->GetLeft(), left)) return false;
        if (!TryEvalConstInt(bin->GetRight(), right)) return false;

        cOpNode *op = bin->GetOp();
        if (op == nullptr) return false;

        switch (op->GetOp())
        {
            case '+': value = left + right; return true;
            case '-': value = left - right; return true;
            case '*': value = left * right; return true;
            case '/':
                if (right == 0) return false;
                value = left / right;
                return true;
            case '%':
                if (right == 0) return false;
                value = left % right;
                return true;
            default:
                return false;
        }
    }

    bool IsCharSizedIntLiteral(cExprNode *expr)
    {
        long long value = 0;
        if (!TryEvalConstInt(expr, value)) return false;
        return (value >= 0 && value <= 127);
    }

    bool IsCharLikeExpr(cExprNode *expr)
    {
        if (expr == nullptr) return false;

        cDeclNode *type = expr->GetType();
        if (type != nullptr && type->IsChar()) return true;

        if (IsCharSizedIntLiteral(expr)) return true;

        cBinaryExprNode *bin = dynamic_cast<cBinaryExprNode*>(expr);
        if (bin != nullptr)
        {
            cDeclNode *binType = bin->GetType();
            if (binType != nullptr && binType->IsFloat()) return false;
            return IsCharLikeExpr(bin->GetLeft()) && IsCharLikeExpr(bin->GetRight());
        }

        return false;
    }

    bool CanAssignExpr(cDeclNode *lhsType, cExprNode *rhs, cDeclNode *rhsType)
    {
        if (lhsType == nullptr || rhs == nullptr || rhsType == nullptr) return false;

        if (lhsType->IsChar())
        {
            if (rhsType->IsChar()) return true;
            if (rhsType->IsFloat()) return false;

            if (IsCharSizedIntLiteral(rhs)) return true;
            return IsCharLikeExpr(rhs);
        }

        return lhsType->IsCompatibleWith(rhsType);
    }
}

void cSemantics::Visit(cAssignNode *node)
{
    cExprNode *lhs = node->GetLHS();
    cExprNode *rhs = node->GetRHS();

    if (lhs != nullptr) lhs->Visit(this);
    if (rhs != nullptr) rhs->Visit(this);

    if (lhs == nullptr || rhs == nullptr) return;

    cVarExprNode *lhsVar = dynamic_cast<cVarExprNode*>(lhs);
    if (lhsVar == nullptr)
    {
        cFuncCallNode *lhsCall = dynamic_cast<cFuncCallNode*>(lhs);
        if (lhsCall != nullptr)
        {
            node->SemanticError(lhsCall->GetName() + " is not an lval");
        }
        return;
    }

    cDeclNode *lhsDecl = lhsVar->GetDecl();
    if (lhsDecl != nullptr && lhsDecl->IsFunc())
    {
        node->SemanticError(lhsVar->GetName() + " is not an lval");
        return;
    }

    cDeclNode *lhsType = lhs->GetType();
    cDeclNode *rhsType = rhs->GetType();

    if (lhsType == nullptr || rhsType == nullptr) return;

    if (!CanAssignExpr(lhsType, rhs, rhsType))
    {
        node->SemanticError("Cannot assign " + rhsType->GetName() +
                            " to " + lhsType->GetName());
    }
}

void cSemantics::Visit(cVarExprNode *node)
{
    cDeclNode *decl = node->GetDecl();
    if (decl == nullptr) return;

    cDeclNode *type = decl->GetType();
    string prefix = node->GetName();

    for (int i = 0; i < node->GetPartCount(); i++)
    {
        cSymbol *fieldSym = node->GetPartSymbol(i);
        if (fieldSym != nullptr)
        {
            if (type == nullptr || !type->IsStruct())
            {
                node->SemanticError(prefix + " is not a struct");
                return;
            }

            cDeclNode *fieldDecl = FindFieldDecl(type, fieldSym->GetName());
            if (fieldDecl == nullptr)
            {
                node->SemanticError(fieldSym->GetName() + " is not a field of " + prefix);
                return;
            }

            type = fieldDecl->GetType();
            prefix += "." + fieldSym->GetName();
            continue;
        }

        cExprNode *indexExpr = node->GetPartExpr(i);
        if (indexExpr != nullptr)
        {
            indexExpr->Visit(this);

            if (type == nullptr || !type->IsArray())
            {
                node->SemanticError(prefix + " is not an array");
                return;
            }

            cDeclNode *indexType = indexExpr->GetType();
            if (indexType != nullptr && !indexType->IsInt())
            {
                node->SemanticError("Index of " + prefix + " is not an int");
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
    cParamsNode *actuals = node->GetParams();
    if (actuals != nullptr) actuals->Visit(this);

    cDeclNode *decl = node->GetDecl();
    if (decl == nullptr) return;

    string name = node->GetName();

    if (!decl->IsFunc())
    {
        node->SemanticError(name + " is not a function");
        return;
    }

    cFuncDeclNode *funcDecl = dynamic_cast<cFuncDeclNode*>(decl);
    if (funcDecl == nullptr) return;

    if (!funcDecl->HasDefinition())
    {
        node->SemanticError(name + " is not fully defined");
        return;
    }

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

        if (!CanAssignExpr(formalType, actual, actualType))
        {
            node->SemanticError("function " + name + " called with incompatible argument");
            break;
        }
    }
}
