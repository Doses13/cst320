//**************************************
// main.cpp
//
// Main function for lang compiler
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "cSymbolTable.h"
#include "lex.h"
#include "astnodes.h"
#include "langparse.h"
#include "cSizeOffset.h"
#include "cCodeGen.h"
#include "cBaseTypeNode.h"

#define DEBUG
#define STACKL
#define STACKL_OPCODES
//#define STACKL_DBG

// define global variables
//cSymbolTable g_SymbolTable;
//long long cSymbol::nextId;

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

// takes two string args: input_file, and output_file
int main(int argc, char **argv)
{
    std::cout << "Joseph Byers" << std::endl;

    std::string outfile_name;
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
    } else {
        outfile_name = "langout";
    }

    g_symbolTable.IncreaseScope();

    AddBaseType("char", 1, false);
    AddBaseType("int", 4, false);
    AddBaseType("float", 4, true);
    AddBaseType("long", 8, false);
    AddBaseType("double", 8, true);

    result = yyparse();
    if (yyast_root != nullptr)
    {
        if (result == 0)
        {
            // NOTE: we should run the semantic error checker here,
            // but not everyone got it to work, so we'll skip it.
            // If yours works, feel free to include it
            //
            // cSemantic semantics;
            // semantics.VisitAllNodes(yyast_root);
            // result += semantics.NumErrors();
            //
            if (result == 0)
            {
                cSizeOffset sizer;
                sizer.VisitAllNodes(yyast_root);

                // need to make the coder go out of scope before assembling
                {
                    cCodeGen coder(outfile_name + ".sl");
                    coder.VisitAllNodes(yyast_root);
                }

                std::string asmCmd =
                    "wsl bash -lc \"cd /mnt/c/Users/shygu/source/repos/cst320/lab7 && ./bin/slasm "
                    + outfile_name + ".sl io320.sl\"";
                system(asmCmd.c_str());
#ifdef STACKL
                std::string runCmd =
                    "wsl bash -lc \"cd /mnt/c/Users/shygu/source/repos/cst320/lab7 && ./bin/stackl "
                    + outfile_name + "\"";
                system(runCmd.c_str());
#endif //STACKL

#ifdef STACKL_OPCODES
                std::cout << "Running with opcodes...\n";
                std::string runCmdOp =
                    "wsl bash -lc \"cd /mnt/c/Users/shygu/source/repos/cst320/lab7 && ./bin/stackl -opcodes "
                    + outfile_name + "\"";
                system(runCmdOp.c_str());
#endif //STACKL_OPCODES

#ifdef STACKL_DBG
                std::cout << "Running with debug information...\n";
                std::string runCmdDbg =
                    "wsl bash -lc \"cd /mnt/c/Users/shygu/source/repos/cst320/lab7 && ./bin/stackl -dbg "
                    + outfile_name + "\"";
                system(runCmdDbg.c_str());
#endif //STACKL_DBG
            }
        } 

        if (result != 0)
        {
            std::cerr << yynerrs << " Errors in compile\n";
        }
    }

    if (result == 0 && yylex() != 0)
    {
        std::cerr << "Junk at end of program\n";
    }

    return result;
}
