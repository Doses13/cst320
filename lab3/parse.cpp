#include <iostream>
#include <string.h>
#include "lex.h"
#include "parse.h"
#include "utils.h"
#include "tokens.h"

// Helper: detect end keyword (your scanner returns IDENTIFIER and yytext == "end")
static bool IsEnd()
{
    return PeekToken() == IDENTIFIER && yytext != nullptr && strcmp(yytext, "end") == 0;
}

static bool Match(int token, const char* expected)
{
    if (PeekToken() == token)
    {
        AdvanceToken();
        return true;
    }
    Error(expected);
    return false;
}

static void SyncToSemicolon()
{
    // Panic-mode: skip until ';' or end or EOF
    while (true)
    {
        int t = PeekToken();
        if (t == 0 || t == SEMICOLON || IsEnd()) break;
        AdvanceToken();
    }
    if (PeekToken() == SEMICOLON) AdvanceToken(); // consume sync token
}

// PROG -> STMTS end
bool FindPROG()
{
    if (!FindSTMTS())
    {
        Error("Program");
        // try to finish cleanly
        while (PeekToken() != 0 && !IsEnd()) AdvanceToken();
        if (IsEnd()) AdvanceToken();
        return false;
    }

    if (!IsEnd())
    {
        Error("'end'");
        return false;
    }

    AdvanceToken(); // consume end
    //std::cout << "Found a Program\n";
    return true;
}

// STMTS -> STMT STMTS | λ
bool FindSTMTS()
{
    // λ when next is end or EOF
    while (!IsEnd() && PeekToken() != 0)
    {
        if (!FindSTMT())
        {
            // error recovery
            SyncToSemicolon();
        }
    }
    return true;
}

// STMT -> EXPR ; | identifier = EXPR ;
bool FindSTMT()
{
    // Disambiguate assignment: identifier '=' ...
    // Note: grammar says "identifier" token here (not {identifier})
    if (PeekToken() == IDENTIFIER)
    {
        int idTok = GetToken(); // consume identifier

        if (PeekToken() == ASSIGN)
        {
            AdvanceToken(); // '='

            if (!FindEXPR())
            {
                return false;
            }

            if (!Match(SEMICOLON, "';'"))
                return false;

            std::cout << "Found a statement\n";
            return true;
        }

        // Not assignment; put identifier back and parse as EXPR ;
        UngetToken(idTok);
    }

    if (!FindEXPR())
        return false;

    if (!Match(SEMICOLON, "';'"))
        return false;

    std::cout << "Found a statement\n";
    return true;
}

// EXPR -> (EXPR) EXPR’ | TERM
bool FindEXPR()
{
    if (PeekToken() == LPAREN)
    {
        AdvanceToken(); // '('

        if (!FindEXPR())
        {
            return false;
        }

        if (!Match(RPAREN, "')'"))
            return false;

        if (!FindEXPR_P())
            return false;

        return true;
    }

    return FindTERM();
}

// EXPR’ -> PLUSOP (EXPR) EXPR’ | λ
bool FindEXPR_P()
{
    // λ if next token isn't + or -
    while (PeekToken() == PLUS || PeekToken() == MINUS)
    {
        if (!FindPLUSOP())
            return false;

        // MUST be (EXPR) per grammar
        if (!Match(LPAREN, "'('"))
            return false;

        if (!FindEXPR())
        {
            return false;
        }

        if (!Match(RPAREN, "')'"))
            return false;

        // tail recursion continues
    }
    return true;
}

// PLUSOP -> + | -
bool FindPLUSOP()
{
    if (PeekToken() == PLUS || PeekToken() == MINUS)
    {
        AdvanceToken();
        return true;
    }
    Error("'+' or '-'");
    return false;
}

// TERM -> [EXPR] TERM’ | num | { identifier }
bool FindTERM()
{
    if (PeekToken() == LBRACKET)
    {
        AdvanceToken(); // '['

        if (!FindEXPR())
        {
            return false;
        }

        if (!Match(RBRACKET, "']'"))
            return false;

        if (!FindTERM_P())
            return false;

        return true;
    }

    if (PeekToken() == INT_VAL) // num
    {
        AdvanceToken();
        return true;
    }

    if (PeekToken() == OPEN) // '{'
    {
        AdvanceToken();

        if (!Match(IDENTIFIER, "'identifier'"))
            return false;

        if (!Match(CLOSE, "'}'"))
            return false;

        return true;
    }

    Error("Term");
    return false;
}

// TERM’ -> TIMESOP [EXPR] TERM’ | λ
bool FindTERM_P()
{
    while (PeekToken() == MULTIPLY || PeekToken() == DIVIDE)
    {
        if (!FindTIMESOP())
            return false;

        // MUST be [EXPR] per grammar
        if (!Match(LBRACKET, "'['"))
            return false;

        if (!FindEXPR())
        {
            return false;
        }

        if (!Match(RBRACKET, "']'"))
            return false;

        // tail recursion continues
    }
    return true;
}

// TIMESOP -> * | /
bool FindTIMESOP()
{
    if (PeekToken() == MULTIPLY || PeekToken() == DIVIDE)
    {
        AdvanceToken();
        return true;
    }
    Error("'*' or '/'");
    return false;
}
