#pragma once
//**************************************
// cBinaryExprNode.h
//
// AST node for a binary expression like a+b or x*y
//

#include "cExprNode.h"
#include "cOpNode.h"
#include "cSymbolTable.h"

class cBinaryExprNode : public cExprNode
{
    public:
        cBinaryExprNode(cExprNode *left, cOpNode *op, cExprNode *right)
            : cExprNode()
        {
            AddChild(left);
            AddChild(op);
            AddChild(right);
        }

        virtual string NodeType() { return string("expr"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }

        cExprNode *GetLeft()  { return static_cast<cExprNode*>(GetChild(0)); }
        cOpNode   *GetOp()    { return static_cast<cOpNode*>(GetChild(1)); }
        cExprNode *GetRight() { return static_cast<cExprNode*>(GetChild(2)); }

        virtual cDeclNode *GetType()
        {
            cExprNode *left = GetLeft();
            cExprNode *right = GetRight();
            cOpNode *op = GetOp();

            if (left == nullptr || right == nullptr || op == nullptr) return nullptr;

            int opcode = op->GetOp();
            if (opcode == '>' || opcode == '<' || opcode == GE || opcode == LE ||
                opcode == EQUALS || opcode == NOT_EQUALS || opcode == AND || opcode == OR)
            {
                cSymbol *intSym = g_symbolTable.Find("int");
                return (intSym == nullptr) ? nullptr : intSym->GetDecl();
            }

            cDeclNode *leftType = left->GetType();
            cDeclNode *rightType = right->GetType();
            if (leftType == nullptr || rightType == nullptr) return nullptr;

            if (leftType->GetName() == rightType->GetName()) return leftType;

            auto builtin = [](const string &name) -> cDeclNode*
            {
                cSymbol *sym = g_symbolTable.Find(name);
                return (sym == nullptr) ? nullptr : sym->GetDecl();
            };

            // any floating-point operand promotes the result to floating-point
            if (leftType->IsFloat() || rightType->IsFloat() ||
                leftType->GetName() == "double" || rightType->GetName() == "double")
            {
                if (leftType->GetName() == "double" || rightType->GetName() == "double")
                    return builtin("double");
                return builtin("float");
            }

            // integer promotions: char < int < long
            if (leftType->GetName() == "long" || rightType->GetName() == "long")
                return builtin("long");
            if (leftType->GetName() == "int" || rightType->GetName() == "int")
                return builtin("int");
            return builtin("char");
        }
};
