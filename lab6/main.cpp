//**************************************
// main.cpp
//
// Main function for lang compiler
//

#define LAB5B

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include "cSymbolTable.h"
#include "lex.h"
#include "astnodes.h"
#include "langparse.h"
#include "cSymbol.h"
#include "cBaseTypeNode.h"
#include "cSizeOffset.h"

#ifdef LAB5B
#include "cSemantics.h"
#endif

static void AddBaseType(const std::string &name, int size, bool isFloat)
{
    cSymbol *sym = g_symbolTable.FindLocal(name);
    if (sym == nullptr)
    {
        sym = new cSymbol(name);
        g_symbolTable.Insert(sym);
    }

    if (sym->GetDecl() == nullptr)
    {
        sym->SetDecl(new cBaseTypeNode(name, size, isFloat));
    }
}

static void InitBuiltInTypes()
{
    g_symbolTable.IncreaseScope();

    AddBaseType("char", 1, false);
    AddBaseType("int", 4, false);
    AddBaseType("float", 4, true);
    AddBaseType("long", 8, false);
    AddBaseType("double", 8, true);
}

int main(int argc, char **argv)
{
    //std::cout << "Joseph Byers" << std::endl;

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

    // Only redirect output if the caller provided an output filename.
    if (argc > 2)
    {
        const char *outfile_name = argv[2];

        FILE *output = fopen(outfile_name, "w");
        if (output == nullptr)
        {
            std::cerr << "Unable to open output file " << outfile_name << "\n";
            exit(-1);
        }

        int output_fd = fileno(output);
        if (dup2(output_fd, 1) < 0)
        {
            std::cerr << "Unable to configure output stream\n";
            exit(-1);
        }
    }

    InitBuiltInTypes();

    result = yyparse();
    if (yyast_root != nullptr && result == 0)
    {
#ifdef LAB5B
        cSemantics semantics;
        semantics.VisitAllNodes(yyast_root);
#endif

        result += yynerrs;
        if (result == 0)
        {
            cSizeOffset sizeOffset;
            sizeOffset.VisitAllNodes(yyast_root);

            std::cout << "<?xml version=\"1.0\"?>\n";
            std::cout << yyast_root->ToString() << std::endl;
        }
    }

    if (yynerrs != 0)
    {
        std::cout << yynerrs << " Errors in compile\n";
    }

    if (result == 0 && yylex() != 0)
    {
        std::cerr << "Junk at end of program\n";
    }

    return result;
}
