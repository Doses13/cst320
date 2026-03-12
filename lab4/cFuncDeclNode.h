#pragma once
//**************************************
// cFuncDeclNode.h
//
// Function declaration / definition node
//

#include "cDeclNode.h"
#include "cSymbol.h"
#include "cArgsNode.h"
#include "cDeclsNode.h"
#include "cStmtsNode.h"

class cFuncDeclNode : public cDeclNode
{
    public:
        cFuncDeclNode(cSymbol *type, cSymbol *name, cArgsNode *args,
                      cDeclsNode *decls, cStmtsNode *stmts) : cDeclNode()
        {
            AddChild(type);
            AddChild(name);
            AddChild(args);
            AddChild(decls);
            AddChild(stmts);
        }

        virtual string NodeType() { return string("func"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};