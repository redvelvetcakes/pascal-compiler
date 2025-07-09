//*****************************************************************************
// purpose: Recursive descent parsing of arithmetic expressions
//          Builds the parse tree while parsing the input
// version: Spring 2025
//  author: Delvin Buckley
//*****************************************************************************

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "nodes.h"
#include <stdlib.h>
#include <iostream>
#include <map>


extern "C" {
  extern FILE *yyin;       // input stream
  extern int   yylex();    // the generated lexical analyzer
  extern char *yytext;     // text of current lexeme
}
extern int nextToken;        // next token returned by lexer

typedef std::map<std::string, float> symbolTableT;
extern symbolTableT symbolTable; // Holds variable names and their types and values

/* Function declarations */
int lex();                   // return the next token

ProgramNode* program();      // parse a program
BlockNode* block();        // parse a block
StatementNode* statement();  // parse a statement
AssignmentNode* assignment_statement(); // parse an assignment statement
CompoundNode* compound_statement(); // parse a compound statement
IfNode* if_statement();      // parse an if statement
WhileNode* while_statement(); // parse a while statement
ReadNode* read_statement();  // parse a read statement
WriteNode* write_statement(); // parse a write statement
ExpressionNode* expression(); // parse an expression
SimpleExpressionNode* simple_expression(); // parse a simple expression
TermNode* term();           // parse a term
FactorNode* factor();       // parse a factor


#endif /* PARSER_H */
