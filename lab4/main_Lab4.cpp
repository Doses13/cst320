//**************************************
// main.cpp
//
// Main function for lang compiler
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include "lex.h"
#include "astnodes.h"
#include "cSymbolTable.h"
#include "langparse.h"

cSymbolTable g_symbolTable;
long long cSymbol::nextId = 0;

extern cAstNode *yyast_root;

static void InsertBuiltInType(const char *name)
{
    cSymbol *sym = new cSymbol(name);
    sym->SetType(true);
    g_symbolTable.Insert(sym);
}

int main(int argc, char **argv)
{
    const char *outfile_name;
    int result = 0;

    if (argc > 1)
    {
        yyin = fopen(argv[1], "r");
        if (yyin == nullptr)
        {
            std::cerr << "ERROR: Unable to open file " << argv[1] << "\n";
            exit(-1);
        }
    }

    if (argc > 2)
    {
        outfile_name = argv[2];

        FILE *output = fopen(outfile_name, "w");
        if (output == nullptr)
        {
            std::cerr << "Unable to open output file " << outfile_name << "\n";
            exit(-1);
        }

        int output_fd = fileno(output);
        if (dup2(output_fd, 1) != 1)
        {
            std::cerr << "Unable configure output stream\n";
            exit(-1);
        }
    }

    // outermost scope for built-in types
    g_symbolTable.IncreaseScope();

    // must be inserted in this exact order for test4 IDs
    InsertBuiltInType("char");
    InsertBuiltInType("int");
    InsertBuiltInType("float");
    InsertBuiltInType("long");
    InsertBuiltInType("double");

    result = yyparse();

    if (result == 0 && yyast_root != nullptr)
    {
        std::cout << "<?xml version=\"1.0\"?>\n";
        std::cout << yyast_root->ToString();
    }
    else if (result != 0)
    {
        std::cout << "Errors in compile\n";
    }

    if (result == 0 && yylex() != 0)
    {
        std::cout << "Junk at end of program\n";
    }

    return result;
}