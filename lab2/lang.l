%{
//**************************************
// lang.l
//
// scanner definition file. flex uses this file to create the scanner
//
// Author: <<<< insert your name here >>>>
//

#include "lex.h"
#include "tokens.h"
#include "cSymbol.h"
#include "cSymbolTable.h"

static cSymbol* ProcessID(const char* name);

#ifdef DEBUG_OUTPUT
    #define DO_RETURN(a) { return Return(a); }
#else
    #define DO_RETURN(a) { return (a); }
#endif

int Return(int val);

static cSymbol* ProcessIdentifier(const char* name)
{
    // LOCAL lookup for this lab
    cSymbol* sym = g_symbolTable.FindLocal(name);

    if (sym == nullptr)
    {
        sym = new cSymbol(name);
        g_symbolTable.Insert(sym);
    }

    return sym;
}

%}

%option noyywrap
%option noinput
%option nounput
%option yylineno

%%

[\t\n\r ]+                      {} /* Ignore whitespace */
"//".*                          {} /* Ignore comments */

"program"                       DO_RETURN(PROGRAM)
"if"                            DO_RETURN(IF)
"else"                          DO_RETURN(ELSE)
"endif"                         DO_RETURN(ENDIF)
"while"                         DO_RETURN(WHILE)
"print"                         DO_RETURN(PRINT)
"char"                          DO_RETURN(CHAR)    
"int"                           DO_RETURN(INT)
"long"                          DO_RETURN(LONG)
"float"                         DO_RETURN(FLOAT)
"double"                        DO_RETURN(DOUBLE)
"struct"                        DO_RETURN(STRUCT)
"array"                         DO_RETURN(ARRAY)
"return"                        DO_RETURN(RETURN)
"prints"                        DO_RETURN(PRINTS)
"local"                         DO_RETURN(LOCAL)
"global"                        DO_RETURN(GLOBAL)
"lookup"                        DO_RETURN(LOOKUP)
"insert"                        DO_RETURN(INSERT)

"!="                            DO_RETURN(NOT_EQUALS)
"=="                            DO_RETURN(EQUALS)
"&&"                            DO_RETURN(AND)
"||"                            DO_RETURN(OR)
">="                            DO_RETURN(GE)
"<="                            DO_RETURN(LE)
"."                             DO_RETURN(DOT)
";"                             DO_RETURN(SEMICOLON)
"="                             DO_RETURN(ASSIGN)                             
"+"                             DO_RETURN(PLUS)
"-"                             DO_RETURN(MINUS)
"*"                             DO_RETURN(MULTIPLY)
"/"                             DO_RETURN(DIVIDE)
"%"                             DO_RETURN(MODULO)
"<"                             DO_RETURN(LT)
">"                             DO_RETURN(GT)
"("                             DO_RETURN(LPAREN)
")"                             DO_RETURN(RPAREN)
","                             DO_RETURN(COMMA)
"["                             DO_RETURN(LBRACKET)
"]"                             DO_RETURN(RBRACKET)

"{" {
    g_symbolTable.IncreaseScope();
    DO_RETURN(OPEN)
}

"}" {
    g_symbolTable.DecreaseScope();
    DO_RETURN(CLOSE)
}

"[A-Za-z0-9_]*"                 DO_RETURN(STRING_LIT)
[1-9][0-9]*"."[0-9]*[1-9]       DO_RETURN(FLOAT_VAL)                  
0|[1-9][0-9]*                   DO_RETURN(INT_VAL)


[A-Za-z_][A-Za-z0-9_]* {
    cSymbol* sym = ProcessID(yytext);
    yylval.symbol = sym;
    DO_RETURN(IDENTIFIER);
}

.                  DO_RETURN(JUNK_TOKEN)

%%

// **********************************************

// This function allows us to do extra processing on each token
// It is used to generate lexer debug info
int Return(int val)
{
    printf("Scanned '%s': %d\n", yytext, val);
    return val;
}

// ProcessID: create symbols in symbol table
static cSymbol* ProcessID(const char* name)
{
    // LOCAL lookup for this lab
    cSymbol* sym = g_symbolTable.FindLocal(name);

    if (sym == nullptr)
    {
        sym = new cSymbol(name);
        g_symbolTable.Insert(sym);
    }

    return sym;
}
