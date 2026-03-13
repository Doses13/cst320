%{
//**************************************
// lang.l
//
// scanner definition file
//

#include <cstdlib>
#include <string>
#include "lex.h"
#include "astnodes.h"
#include "cSymbolTable.h"
#include "langparse.h"

static cSymbol* ProcessID(const char* name);

#ifdef DEBUG_OUTPUT
    #define DO_RETURN(a) { return Return(a); }
#else
    #define DO_RETURN(a) { return (a); }
#endif

int Return(int val);
%}

%option noyywrap
%option noinput
%option nounput
%option yylineno

%%

[\t\n\r ]+              {}
"//".*                  {}

"program"               DO_RETURN(PROGRAM)
"array"                 DO_RETURN(ARRAY)
"struct"                DO_RETURN(STRUCT)
"if"                    DO_RETURN(IF)
"else"                  DO_RETURN(ELSE)
"endif"                 DO_RETURN(ENDIF)
"while"                 DO_RETURN(WHILE)
"print"                 DO_RETURN(PRINT)
"prints"                DO_RETURN(PRINTS)
"return"                DO_RETURN(RETURN)

"char"                  { yylval.symbol = g_symbolTable.Find("char");   DO_RETURN(TYPE_ID) }
"int"                   { yylval.symbol = g_symbolTable.Find("int");    DO_RETURN(TYPE_ID) }
"float"                 { yylval.symbol = g_symbolTable.Find("float");  DO_RETURN(TYPE_ID) }
"long"                  { yylval.symbol = g_symbolTable.Find("long");   DO_RETURN(TYPE_ID) }
"double"                { yylval.symbol = g_symbolTable.Find("double"); DO_RETURN(TYPE_ID) }

"=="                    DO_RETURN(EQUALS)
"!="                    DO_RETURN(NOT_EQUALS)
">="                    DO_RETURN(GE)
"<="                    DO_RETURN(LE)
"&&"                    DO_RETURN(AND)
"||"                    DO_RETURN(OR)

\"([^\\\"]|\\.)*\"      {
                            std::string s(yytext);
                            s = s.substr(1, s.size() - 2);
                            yylval.str_val = new std::string(s);
                            DO_RETURN(STRING_LIT)
                        }

"{"                     DO_RETURN('{')
"}"                     DO_RETURN('}')
"("                     DO_RETURN('(')
")"                     DO_RETURN(')')
"["                     DO_RETURN('[')
"]"                     DO_RETURN(']')
";"                     DO_RETURN(';')
","                     DO_RETURN(',')
"."                     DO_RETURN('.')
"+"                     DO_RETURN('+')
"-"                     DO_RETURN('-')
"*"                     DO_RETURN('*')
"/"                     DO_RETURN('/')
"%"                     DO_RETURN('%')
"="                     DO_RETURN('=')
">"                     DO_RETURN('>')
"<"                     DO_RETURN('<')

[0-9]+\.[0-9]+          {
                            yylval.float_val = atof(yytext);
                            DO_RETURN(FLOAT_VAL)
                        }

0|[1-9][0-9]*           {
                            yylval.int_val = atoi(yytext);
                            DO_RETURN(INT_VAL)
                        }

[A-Za-z_][A-Za-z0-9_]*  {
                            cSymbol* sym = ProcessID(yytext);
                            yylval.symbol = sym;

                            if (sym != nullptr && sym->GetDecl() != nullptr && sym->GetDecl()->IsType())
                                DO_RETURN(TYPE_ID)
                            else
                                DO_RETURN(IDENTIFIER)
                        }

.                       DO_RETURN(JUNK_TOKEN)

%%

int Return(int val)
{
    printf("Scanned '%s': %d\n", yytext, val);
    return val;
}

static cSymbol* ProcessID(const char* name)
{
    cSymbol* sym = g_symbolTable.Find(name);

    if (sym == nullptr)
    {
        sym = new cSymbol(name);
        g_symbolTable.Insert(sym);
    }

    return sym;
}
