/*********************************************************************
    Name: Delvin Buckley   NetID: db2847
    Course: CSE 4714    Assignment: Part 1
    Programming Environment: Visual Studio 
    Purpose of File: Contains the rules for a lexical analyzer for a subset of the language TIPS
*********************************************************************/
%option yylineno
%option noyywrap
%{
#include "lexer.h"
// global variable to hold current line number being read
int line_number = 1;
%}

COMMENT \{[^\{\}\n]*\}
IDENTIFIER [A-Z][A-Z0-9]*
DIGIT [0-9]
STRINGLIT \"([^\\\"\n]|\\.)*\"|\'([^\\'\n]|\\.)*\'

%%

 /* Found a BEGIN */
BEGIN     { return TOK_BEGIN; }

 /* Found a BREAK */
BREAK     { return TOK_BREAK; }

 /* Found a CONTINUE */
CONTINUE  { return TOK_CONTINUE; }

 /* Found a DOWNTO */
DOWNTO    { return TOK_DOWNTO; }

 /* Found an ELSE */
ELSE      { return TOK_ELSE; }

 /* Found an END */
END       { return TOK_END; }

 /* Found a FOR */
FOR       { return TOK_FOR; }

 /* Found an IF */
IF        { return TOK_IF; }

 /* Found a LET */
LET       { return TOK_LET; }

 /* Found a PROGRAM */
PROGRAM   { return TOK_PROGRAM; }

 /* Found a READ */
READ      { return TOK_READ; }

 /* Found a THEN */
THEN      { return TOK_THEN; }

 /* Found a TO */
TO        { return TOK_TO; }

 /* Found a VAR */
VAR       { return TOK_VAR; }

 /* Found a WHILE */
WHILE     { return TOK_WHILE; }

 /* Found a WRITE */
WRITE     { return TOK_WRITE; }

 /* Found an INTEGER */
INTEGER   { return TOK_INTEGER; }

 /* Found a REAL */
REAL      { return TOK_REAL; }

 /* Found an ASSIGN */
:=        { return TOK_ASSIGN; }

 /* Found an EQUAL TO */
=         { return TOK_EQUALTO; }

 /* Found a SEMICOLON */
;         { return TOK_SEMICOLON; }

 /* Found a COLON */
:         { return TOK_COLON; }

 /* Found an OPEN PARENTHESIS */
"("       { return TOK_OPENPAREN; }

 /* Found a CLOSE PARENTHESIS */
")"       { return TOK_CLOSEPAREN; }

 /* Found a PLUS */
"+"       { return TOK_PLUS; }

 /* Found a MINUS */
-         { return TOK_MINUS; }

 /* Found a MULTIPLY */
"*"       { return TOK_MULTIPLY; }

 /* Found a DIVIDE */
"/"       { return TOK_DIVIDE; }

 /* Found a NOT EQUAL TO */
"<>"      { return TOK_NOTEQUALTO; }

 /* Found a LESS THAN */
"<"       { return TOK_LESSTHAN; }

 /* Found a GREATER THAN */
">"       { return TOK_GREATERTHAN; }

 /* Found a MOD */
MOD       { return TOK_MOD; }

 /* Found a Not */
NOT       { return TOK_NOT; }

 /* Found an OR */
OR        { return TOK_OR; }

 /* Found an AND */
AND       { return TOK_AND; }

 /* Found an IDENTIFIER */
{IDENTIFIER}    { if (yyleng <= 8) return TOK_IDENT; else return TOK_UNKNOWN; }

 /*Found a FLOATING POINT LITERAL */
{DIGIT}+\.{DIGIT}+ { return TOK_FLOATLIT; }

 /* Found an INTEGER LITERAL */
{DIGIT}+    { return TOK_INTLIT; }

 /* Found a STRING LITERAL */
{STRINGLIT} { if (yyleng <= 80) return TOK_STRINGLIT; else return TOK_UNKNOWN; }

 /* Ignore comments */
{COMMENT}  ;

 /* Eat any whitespace*/
[ \t\r]+   ;

 /* Increment line count */
\n        { line_number++; }

 /* Found an unknown character */
.         { return TOK_UNKNOWN; }

 /* Recognize end of file */
<<EOF>>   { return TOK_EOF ;}

%%
