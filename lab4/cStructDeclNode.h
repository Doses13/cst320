#pragma once
//**************************************
// cStructDeclNode.h
//
// AST node for struct declarations
//

#include "cDeclNode.h"
#include "cDeclsNode.h"
#include "cSymbol.h"

class cStructDeclNode : public cDeclNode
{
    public:
        cStructDeclNode(cDeclsNode *fields, cSymbol *name) : cDeclNode()
        {
            AddChild(fields);
            AddChild(name);
        }

        virtual string NodeType() { return string("struct_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};