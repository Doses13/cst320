#pragma once

//**************************************
// tokens.h
//
// Defines the values for the tokens returned by yylex
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#define DOT         46          // .
#define ASSIGN      61          // =
#define PLUS        43          // +
#define MINUS       45          // -
#define MULTIPLY    42          // *
#define DIVIDE      47          // /
#define MODULO      37          // %
#define LT          60          // <
#define GT          62         // >
#define LPAREN      40          // (
#define RPAREN      41          // )
#define COMMA       44          // ,
#define LBRACKET    91          // [
#define RBRACKET    93          // ]
#define SEMICOLON   59          // ;
#define PROGRAM     1000        // program
#define IF          1001        // if
#define ELSE        1002        // else
#define ENDIF       1003        // endif
#define WHILE       1004        // while
#define PRINT       1005        // print
#define CHAR        1006        // char
#define INT         1007        // int
#define LONG        1008        // Wlong
#define FLOAT       1009        // float
#define DOUBLE      1010        // double
#define INT_VAL     1011        // <an integer constant>
#define FLOAT_VAL   1012        // <a float constant>
#define STRUCT      1013        // struct
#define ARRAY       1014        // array
#define RETURN      1015        // return
#define IDENTIFIER  1016        // <a C style identifier>
#define NOT_EQUALS  1017        // !=
#define EQUALS      1018        // ==
#define AND         1019        // &&
#define OR          1020        // ||
#define GE          1021        // >=
#define LE          1022        // <=
#define PRINTS      1023        // prints
#define STRING_LIT  1024        // "stuff..."
#define LOCAL       1025
#define GLOBAL      1026
#define LOOKUP      1027
#define INSERT      1028
#define OPEN        123         // {
#define CLOSE       125         // }
#define JUNK_TOKEN  2000        // used to skip unwanted tokens