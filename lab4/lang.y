%{
//**************************************
// lang.y
//
// Parser definition file.
//

#include <iostream>
#include "lex.h"
#include "astnodes.h"
#include "cSymbolTable.h"

int yyerror(const char *msg);
cAstNode *yyast_root = nullptr;
%}

%locations

%union{
    int             int_val;
    double          float_val;
    std::string*    str_val;

    cAstNode*       ast_node;
    cProgramNode*   program_node;
    cBlockNode*     block_node;
    cDeclsNode*     decls_node;
    cDeclNode*      decl_node;
    cStmtsNode*     stmts_node;
    cStmtNode*      stmt_node;
    cExprNode*      expr_node;
    cVarExprNode*   var_expr_node;
    cArgsNode*      args_node;
    cParamsNode*    params_node;
    cSymbol*        symbol;
}

%start program

%token <symbol>    IDENTIFIER
%token <symbol>    TYPE_ID
%token <int_val>   CHAR_VAL
%token <int_val>   INT_VAL
%token <float_val> FLOAT_VAL
%token <int_val>   LE
%token <int_val>   GE
%token <int_val>   AND
%token <int_val>   OR
%token <int_val>   EQUALS
%token <int_val>   NOT_EQUALS
%token <str_val>   STRING_LIT

%token PROGRAM
%token PRINT PRINTS
%token WHILE IF ELSE ENDIF
%token STRUCT ARRAY
%token RETURN
%token JUNK_TOKEN

%type <program_node> program
%type <block_node>   block
%type <decls_node>   decls
%type <decl_node>    decl
%type <decl_node>    var_decl
%type <decl_node>    array_decl
%type <decl_node>    struct_decl
%type <decl_node>    func_decl
%type <decl_node>    func_def
%type <decl_node>    arg_decl
%type <stmts_node>   stmts
%type <stmt_node>    stmt
%type <expr_node>    expr
%type <expr_node>    equal
%type <expr_node>    addit
%type <expr_node>    term
%type <expr_node>    fact
%type <expr_node>    lval
%type <expr_node>    func_call
%type <var_expr_node> varref
%type <symbol>       varpart
%type <args_node>    args
%type <args_node>    arg_list
%type <params_node>  params
%type <params_node>  param_list

%%

program
    : PROGRAM block
      {
          $$ = new cProgramNode($2);
          yyast_root = $$;

          if (yynerrs == 0) YYACCEPT;
          else YYABORT;
      }
    ;

block
    : open decls stmts close
      {
          $$ = new cBlockNode($2, $3);
      }
    | open stmts close
      {
          $$ = new cBlockNode(nullptr, $2);
      }
    ;

open
    : '{'
      {
          g_symbolTable.IncreaseScope();
      }
    ;

close
    : '}'
      {
          g_symbolTable.DecreaseScope();
      }
    ;

func_scope_open
    :
      {
          g_symbolTable.IncreaseScope();
      }
    ;

func_scope_close
    :
      {
          g_symbolTable.DecreaseScope();
      }
    ;

decls
    : decls decl
      {
          $$ = $1;
          $$->Insert($2);
      }
    | decl
      {
          $$ = new cDeclsNode($1);
      }
    ;

decl
    : var_decl ';'
      {
          $$ = $1;
      }
    | array_decl ';'
      {
          $$ = $1;
      }
    | struct_decl ';'
      {
          $$ = $1;
      }
    | func_decl ';'
      {
          $$ = $1;
      }
    | func_def
      {
          $$ = $1;
      }
    | error ';'
      {
          $$ = nullptr;
      }
    ;

var_decl
    : TYPE_ID IDENTIFIER
      {
          cSymbol *nameSym = nullptr;
          cSymbol *local = g_symbolTable.FindLocal($2->GetName());

          if (local == $2)
          {
              nameSym = $2;
          }
          else
          {
              nameSym = new cSymbol($2->GetName());
              g_symbolTable.Insert(nameSym);
          }

          $$ = new cVarDeclNode($1, nameSym);
      }
    ;

array_decl
    : ARRAY TYPE_ID '[' INT_VAL ']' IDENTIFIER
      {
          cSymbol *nameSym = nullptr;
          cSymbol *local = g_symbolTable.FindLocal($6->GetName());

          if (local == $6)
          {
              nameSym = $6;
          }
          else
          {
              nameSym = new cSymbol($6->GetName());
              g_symbolTable.Insert(nameSym);
          }

          nameSym->SetType(true);
          $$ = new cArrayDeclNode($2, nameSym, $4);
      }
    ;

arg_decl
    : TYPE_ID IDENTIFIER
      {
          cSymbol *nameSym = nullptr;
          cSymbol *local = g_symbolTable.FindLocal($2->GetName());

          if (local == $2)
          {
              nameSym = $2;
          }
          else
          {
              nameSym = new cSymbol($2->GetName());
              g_symbolTable.Insert(nameSym);
          }

          $$ = new cVarDeclNode($1, nameSym);
      }
    ;

args
    :
      {
          $$ = nullptr;
      }
    | arg_list
      {
          $$ = $1;
      }
    ;

arg_list
    : arg_decl
      {
          $$ = new cArgsNode($1);
      }
    | arg_list ',' arg_decl
      {
          $$ = $1;
          $$->Insert($3);
      }
    ;

func_decl
    : TYPE_ID IDENTIFIER func_scope_open '(' args ')' func_scope_close
      {
          $$ = new cFuncDeclNode($1, $2, $5, nullptr, nullptr);
      }
    ;

func_def
    : TYPE_ID IDENTIFIER func_scope_open '(' args ')' open decls stmts close func_scope_close
      {
          $$ = new cFuncDeclNode($1, $2, $5, $8, $9);
      }
    | TYPE_ID IDENTIFIER func_scope_open '(' args ')' open stmts close func_scope_close
      {
          $$ = new cFuncDeclNode($1, $2, $5, nullptr, $8);
      }
    | TYPE_ID IDENTIFIER func_scope_open '(' args ')' open decls close func_scope_close
      {
          $$ = new cFuncDeclNode($1, $2, $5, $8, nullptr);
      }
    | TYPE_ID IDENTIFIER func_scope_open '(' args ')' open close func_scope_close
      {
          $$ = new cFuncDeclNode($1, $2, $5, nullptr, nullptr);
      }
    ;

struct_decl
    : STRUCT open decls close IDENTIFIER
      {
          $5->SetType(true);
          $$ = new cStructDeclNode($3, $5);
      }
    ;

stmts
    : stmts stmt
      {
          $$ = $1;
          $$->Insert($2);
      }
    | stmt
      {
          $$ = new cStmtsNode($1);
      }
    ;

stmt
    : IF '(' expr ')' stmts ENDIF ';'
      {
          $$ = new cIfNode($3, $5);
      }
    | IF '(' expr ')' stmts ELSE stmts ENDIF ';'
      {
          $$ = new cIfNode($3, $5, $7);
      }
    | WHILE '(' expr ')' stmt
      {
          $$ = new cWhileNode($3, $5);
      }
    | PRINT '(' expr ')' ';'
      {
          $$ = new cPrintNode($3);
      }
    | PRINTS '(' STRING_LIT ')' ';'
      {
          $$ = new cPrintsNode(*$3);
          delete $3;
      }
    | lval '=' expr ';'
      {
          $$ = new cAssignNode($1, $3);
      }
    | func_call ';'
      {
          $$ = static_cast<cStmtNode*>($1);
      }
    | block
      {
          $$ = $1;
      }
    | RETURN expr ';'
      {
          $$ = new cReturnNode($2);
      }
    | RETURN '(' expr ')' ';'
      {
          $$ = new cReturnNode($3);
      }
    | error ';'
      {
          $$ = nullptr;
      }
    ;

expr
    : equal
      {
          $$ = $1;
      }
    ;

equal
    : equal EQUALS addit
      {
          $$ = new cBinaryExprNode($1, new cOpNode(EQUALS), $3);
      }
    | equal NOT_EQUALS addit
      {
          $$ = new cBinaryExprNode($1, new cOpNode(NOT_EQUALS), $3);
      }
    | addit
      {
          $$ = $1;
      }
    ;

addit
    : addit '+' term
      {
          $$ = new cBinaryExprNode($1, new cOpNode('+'), $3);
      }
    | addit '-' term
      {
          $$ = new cBinaryExprNode($1, new cOpNode('-'), $3);
      }
    | term
      {
          $$ = $1;
      }
    ;

term
    : term '*' fact
      {
          $$ = new cBinaryExprNode($1, new cOpNode('*'), $3);
      }
    | term '/' fact
      {
          $$ = new cBinaryExprNode($1, new cOpNode('/'), $3);
      }
    | term '%' fact
      {
          $$ = new cBinaryExprNode($1, new cOpNode('%'), $3);
      }
    | fact
      {
          $$ = $1;
      }
    ;

fact
    : '(' expr ')'
      {
          $$ = $2;
      }
    | INT_VAL
      {
          $$ = new cIntExprNode($1);
      }
    | FLOAT_VAL
      {
          $$ = new cFloatExprNode($1);
      }
    | func_call
      {
          $$ = $1;
      }
    | varref
      {
          $$ = $1;
      }
    ;

params
    :
      {
          $$ = nullptr;
      }
    | param_list
      {
          $$ = $1;
      }
    ;

param_list
    : expr
      {
          $$ = new cParamsNode($1);
      }
    | param_list ',' expr
      {
          $$ = $1;
          $$->Insert($3);
      }
    ;

func_call
    : IDENTIFIER '(' params ')'
      {
          $$ = new cFuncCallNode($1, $3);
      }
    ;

varref
    : varref '.' varpart
      {
          $$ = $1;
          $$->AddPart($3);
      }
    | varref '[' expr ']'
      {
          $$ = $1;
          $$->AddPart($3);
      }
    | varpart
      {
          $$ = new cVarExprNode($1);
      }
    ;

varpart
    : IDENTIFIER
      {
          $$ = $1;
      }
    ;

lval
    : varref
      {
          $$ = $1;
      }
    ;

%%

int yyerror(const char *msg)
{
    std::cerr << "ERROR: " << msg
              << " at symbol " << yytext
              << " on line " << yylineno << "\n";
    return 0;
}