#pragma once
//**************************************
// cAstNode.h
//
// pure virtual base class for all AST nodes
//

#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;

#include "cVisitor.h"

// The following are defined in lex.h, but can't include due to circularity
extern int yylineno;        // Need to be able to store line numbers
extern int yynerrs;         // Increment on each semantic error

// Declare the Semantic Error routines used at parse time.
// By declaring them here, all AST node implementations have access to them.
void SemanticParseError(std::string error);
void SemanticParseErrorAt(std::string error, int line);

class cAstNode
{
    public:
        cAstNode() : m_LineNum(yylineno), m_hasSemanticError(false) {}

    protected:
        void AddChild(cAstNode *child)
        {
            m_children.push_back(child);
        }

        void AddAllChildren(cAstNode *node)
        {
            if (node != nullptr && node->HasChildren())
            {
                for (auto it = node->m_children.begin();
                     it != node->m_children.end();
                     it++)
                {
                    AddChild(*it);
                }
            }
        }

        bool HasChildren() { return !m_children.empty(); }
        int NumChildren()  { return (int)m_children.size(); }

        cAstNode* GetChild(int child)
        {
            if (child >= (int)m_children.size()) return nullptr;
            return m_children[child];
        }

        void SetChild(int index, cAstNode *child)
        {
            m_children[index] = child;
        }

        virtual string NodeType() = 0;
        virtual string AttributesToString() { return string(""); }

    public:
        string ToString()
        {
            return ToString(0);
        }

        int GetLineNum() const
        {
            return m_LineNum;
        }

        void SemanticError(string message)
        {
            std::cout << "ERROR: " << message << " near line " << m_LineNum
                      << "\n";
            yynerrs++;
            m_hasSemanticError = true;
        }

        void VisitAllChildren(cVisitor* visitor)
        {
            for (auto it = m_children.begin(); it < m_children.end(); it++)
            {
                if ((*it) != nullptr) (*it)->Visit(visitor);
            }
        }

        virtual void Visit(cVisitor *visitor) = 0;

        bool HasSemanticError()
        {
            if (m_hasSemanticError)
                return true;
            else
            {
                for (auto it = m_children.begin(); it != m_children.end(); it++)
                {
                    if ((*it) != nullptr && (*it)->HasSemanticError())
                    {
                        return true;
                    }
                }
            }

            return false;
        }

    protected:
        string ToString(int indent)
        {
            string pad(indent * 2, ' ');
            string result = pad + "<" + NodeType() + AttributesToString();

            bool hasRealChildren = false;
            for (auto child : m_children)
            {
                if (child != nullptr)
                {
                    hasRealChildren = true;
                    break;
                }
            }

            if (!hasRealChildren)
            {
                result += "/>\n";
                return result;
            }

            result += ">\n";

            for (auto child : m_children)
            {
                if (child != nullptr)
                {
                    result += child->ToString(indent + 1);
                }
            }

            result += pad + "</" + NodeType() + ">\n";
            return result;
        }

    private:
        vector<cAstNode *> m_children;
        int m_LineNum;
        bool m_hasSemanticError;
};
