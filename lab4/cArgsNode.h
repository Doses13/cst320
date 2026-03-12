#pragma once
//**************************************
// cArgsNode.h
//
// List of function argument declarations
//

#include "cAstNode.h"
#include "cDeclNode.h"

class cArgsNode : public cAstNode
{
    public:
        cArgsNode(cDeclNode *decl) : cAstNode()
        {
            AddChild(decl);
        }

        void Insert(cDeclNode *decl)
        {
            AddChild(decl);
        }

        virtual string NodeType() { return string("args"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};