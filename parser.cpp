//*****************************************************************************
// purpose: Recursive descent parsing of arithmetic expressions
//          Builds the parse tree while parsing the input
// version: Spring 2025
//  author: Delvin Buckley
//*****************************************************************************

#include "parser.h"
#include "nodes.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

// Forward declarations of first_of functions

bool first_of_program();            // program should start with TOK_PROGRAM
bool first_of_block();              // block should start with TOK_VAR or TOK_BEGIN
bool first_of_statement();          // statement should start with TOK_IDENT, TOK_BEGIN, TOK_IF, TOK_WHILE, TOK_READ, or TOK_WRITE
bool first_of_compound_statement(); // compound statement should start with TOK_BEGIN
bool first_of_expression();         // expression should start with TOK_IDENT, TOK_INTLIT, TOK_FLOATLIT, or TOK_OPENPAREN
bool first_of_simple_expression();  // simple expression should start with TOK_IDENT, TOK_INTLIT, TOK_FLOATLIT, or TOK_OPENPAREN
bool first_of_term();               // term should start with TOK_IDENT, TOK_INTLIT, TOK_FLOATLIT, or TOK_OPENPAREN
bool first_of_factor();             // factor should start with TOK_IDENT, TOK_INTLIT, TOK_FLOATLIT, or TOK_OPENPAREN

int nextToken = 0;            // hold nextToken returned by lex
bool printParse = false;      // shall we print the parse tree?
bool printTree = false;

//*****************************************************************************
// Holds variable names and their types and values
symbolTableT symbolTable;
// Determine if a symbol is in the symbol table
bool inSymbolTable(string idName) {
  symbolTableT::iterator it;
  it = symbolTable.find(idName);
  // If idName is missing, will be set to the end
  return !(it == symbolTable.end());
}

// Which tree level are we currently in?  Setting this to -1
// means the top-level expression is at level 0.
static int level = -1;

// Handle syntax errors
void error() {
  cout << endl << "===========================" << endl;
  cout << "ERROR near: " << yytext;
  cout << endl << "===========================" << endl;
  if (yyin)
    fclose(yyin);
  exit(EXIT_FAILURE);
}
//*****************************************************************************
// Print each level with appropriate indentation
void indent() {
  for (int i = 0; i<level; i++)
    cout << ("  ");
}
//*****************************************************************************
// Announce what the lexical analyzer has found
void output() {
  indent();
  cout << "---> FOUND " << yytext << endl;
}
//*****************************************************************************
// Read the next token from the input stream
int lex() {
  nextToken = yylex();

  if (nextToken == TOK_EOF) {
      // save a "lexeme" into yytext
      yytext[0] = 'E';
      yytext[1] = 'O';
      yytext[2] = 'F';
      yytext[3] = 0;
  }
  if(printParse) {
    // Tell us about the token and lexeme
    indent();
    cout << "Next token is: ";
    switch(nextToken) {
    case TOK_BEGIN:       cout << "TOK_BEGIN";       break;
    case TOK_BREAK:       cout << "TOK_BREAK";       break;
    case TOK_CONTINUE:    cout << "TOK_CONTINUE";    break;
    case TOK_DOWNTO:      cout << "TOK_DOWNTO";      break;
    case TOK_ELSE:        cout << "TOK_ELSE";        break;
    case TOK_END:         cout << "TOK_END";         break;
    case TOK_FOR:         cout << "TOK_FOR";         break;
    case TOK_IF:          cout << "TOK_IF";          break;
    case TOK_LET:         cout << "TOK_LET";         break;
    case TOK_PROGRAM:     cout << "TOK_PROGRAM";     break;
    case TOK_READ:        cout << "TOK_READ";        break;
    case TOK_THEN:        cout << "TOK_THEN";        break;
    case TOK_TO:          cout << "TOK_TO";          break;
    case TOK_VAR:         cout << "TOK_VAR";         break;
    case TOK_WHILE:       cout << "TOK_WHILE";       break;
    case TOK_WRITE:       cout << "TOK_WRITE";       break;

    case TOK_INTEGER:     cout << "TOK_INTEGER";     break;
    case TOK_REAL:        cout << "TOK_REAL";        break;

    case TOK_SEMICOLON:   cout << "TOK_SEMICOLON";   break;
    case TOK_COLON:       cout << "TOK_COLON";       break;
    case TOK_OPENPAREN:   cout << "TOK_OPENPAREN";   break;
    case TOK_CLOSEPAREN:  cout << "TOK_CLOSEPAREN";  break;

    case TOK_PLUS:        cout << "TOK_PLUS";        break;
    case TOK_MINUS:       cout << "TOK_MINUS";       break;
    case TOK_MULTIPLY:    cout << "TOK_MULTIPLY";    break;
    case TOK_DIVIDE:      cout << "TOK_DIVIDE";      break;
    case TOK_ASSIGN:      cout << "TOK_ASSIGN";      break;
    case TOK_EQUALTO:     cout << "TOK_EQUALTO";     break;
    case TOK_LESSTHAN:    cout << "TOK_LESSTHAN";    break;
    case TOK_GREATERTHAN: cout << "TOK_GREATERTHAN"; break;
    case TOK_NOTEQUALTO:  cout << "TOK_NOTEQUALTO";  break;
    case TOK_MOD:         cout << "TOK_MOD";         break;
    case TOK_NOT:         cout << "TOK_NOT";         break;
    case TOK_OR:          cout << "TOK_OR";          break;
    case TOK_AND:         cout << "TOK_AND";         break;

    case TOK_IDENT:       cout << "TOK_IDENT";       break;
    case TOK_INTLIT:      cout << "TOK_INTLIT";      break;
    case TOK_FLOATLIT:    cout << "TOK_FLOATLIT";    break;
    case TOK_STRINGLIT:   cout << "TOK_STRINGLIT";   break;
    case TOK_UNKNOWN:      cout << "TOK_UNKNOWN";     break;
    
    default: error();
    }
    cout << ", Next lexeme is: " << yytext << endl;
  }
  return nextToken;
}
//*****************************************************************************
// Parses strings in the language generated by the rule:
// <program> → TOK_PROGRAM TOK_IDENT TOK_SEMICOLON <block>
ProgramNode* program() 
{
  if (!first_of_program())
    error();

  if(printParse) {
    indent();
    cout << "Enter <program>" << endl;
  }
  level = level + 1;

  lex(); // Read past TOK_PROGRAM

  string programName;

  if (nextToken == TOK_IDENT) {
    if(printParse) output();
    programName = yytext;
    lex(); // Read past the identifier
  } else {
    error();
  }
  
  if (nextToken == TOK_SEMICOLON) {
    if(printParse) output();
    lex(); // Read past the semicolon
  } else {
    error();
  }
    
  BlockNode* blockPtr = block();

  if (nextToken == TOK_EOF) {
    if(printParse) output();
    // End of file reached. Do nothing.
  }
  else
    error();

  level = level - 1;
  if(printParse) {
    indent();
    cout << "Exit <program>" << endl;
  }

  ProgramNode* newProgramNode = new ProgramNode(level, programName, blockPtr);
  return newProgramNode;
}
bool first_of_program() 
{
  return nextToken == TOK_PROGRAM;
}

//*****************************************************************************
// Parses strings in the language generated by the rule:
// <block> → ( TOK_VAR TOK_IDENT TOK_COLON type TOK_SEMIOLON {[ TOK_IDENT TOK_COLON type TOK_SEMIOLON ]} <compound> ) | <compound>
BlockNode* block() 
{
  if (!first_of_block())
    error();

  if(printParse) {
    indent();
    cout << "Enter <block>" << endl;
  }

  level = level + 1;
  BlockNode* newBlockNode = new BlockNode(level);

  if (nextToken == TOK_VAR) {
    if(printParse) output();
    lex(); // Read past TOK_VAR

    while (nextToken == TOK_IDENT) {
      std::string *varName = new std::string(yytext);
      if(printParse) output();
      if (inSymbolTable(*varName)) {
        error();
      }
      lex(); // Read past the identifier

      if (nextToken == TOK_COLON) {
        if(printParse) output();
        lex(); // Read past the colon
      } else {
        error();
      }

      if (nextToken == TOK_INTEGER || nextToken == TOK_REAL) {
        if(printParse) output(); // Read past the type
      } else {
        error();
      }

      std::string *varType = new std::string(yytext);

      /*newBlockNode->varNames.push_back(varName);
      newBlockNode->varTypes.push_back(varType);*/
      symbolTable.insert(std::pair<std::string, float>(*varName, 0.0f));

      lex(); // Read past the type

      if (nextToken == TOK_SEMICOLON) {
        if (printParse) output();
        lex(); // Read past the semicolon
      } else {
        error();
      }
    }
  }

  newBlockNode->compound = compound_statement();

  level = level - 1;
  if(printParse) {
    indent();
    cout << "Exit <block>" << endl;
  }

  return newBlockNode;
}
bool first_of_block() 
{
  return nextToken == TOK_BEGIN || nextToken == TOK_VAR;
}

//*****************************************************************************
// Parses strings in the language generated by the rule:
// <statement> → <assignment> | <compound> | <if> | <while> | <read> | <write>
StatementNode* statement() 
{
  if (!first_of_statement())
    error();
  
  if(printParse) {
    indent();
    cout << "Enter <statement>" << endl;
  }

  level = level + 1;

  StatementNode* newStatementNode = nullptr;

  switch (nextToken) {
    case TOK_IDENT:
      newStatementNode = assignment_statement();
      break;
    case TOK_BEGIN:
      newStatementNode = compound_statement();
      break;
    case TOK_IF:
      newStatementNode = if_statement();
      break;
    case TOK_WHILE:
      newStatementNode = while_statement();
      break;
    case TOK_READ:
      newStatementNode = read_statement();
      break;
    case TOK_WRITE:
      newStatementNode = write_statement();
      break;
    default:
      error();
  }

  level = level - 1;
  if(printParse) {
    indent();
    cout << "Exit <statement>" << endl;
  }

  return newStatementNode;
}
bool first_of_statement() 
{
  switch (nextToken) {
    case TOK_IDENT:
      return true;
    case TOK_BEGIN:
      return true;
    case TOK_IF:
      return true;
    case TOK_WHILE:
      return true;
    case TOK_READ:
      return true;
    case TOK_WRITE:
      return true;
    default:
      return false;
    }
}

//*****************************************************************************
// Parses strings in the language generated by the rule:
// <assignment> → TOK_IDENT TOK_ASSIGN <expression>
AssignmentNode* assignment_statement() 
{
  if (nextToken != TOK_IDENT)
    error();
  
  string id = string(yytext); // Save the identifier

  if(printParse) {
    indent();
    cout << "Enter <assignment>" << endl;
  }
  level = level + 1;

  if (printParse) output();
  lex(); // Read past the identifier

  if (nextToken == TOK_ASSIGN) {
    if(printParse) output();
    lex(); // Read past the assignment operator
  } else {
    error();
  }

  ExpressionNode* expr = expression();
  AssignmentNode* newAssignmentNode = new AssignmentNode(level, id, expr);
  
  level = level - 1;

  if(printParse) {
    indent();
    cout << "Exit <assignment>" << endl;
  }

  return newAssignmentNode;
}

//*****************************************************************************
// Parses strings in the language generated by the rule:
// <compound> → TOK_BEGIN <statement> { TOK_SEMICOLON <statement> } TOK_END
CompoundNode* compound_statement() 
{
  if (!first_of_compound_statement())
    error();

  if(printParse) {
    indent();
    cout << "Enter <compound>" << endl;
  }

  level = level + 1;

  CompoundNode* newCompoundNode = new CompoundNode(level);

  lex(); // Read past TOK_BEGIN

  newCompoundNode->addStatement(statement()); // Add the first statement

  while (nextToken == TOK_SEMICOLON) {
    if(printParse) output();
    lex(); // Read past the semicolon
    newCompoundNode->addStatement(statement());
  }

  if (nextToken == TOK_END) {
    if(printParse) output();
    lex(); // Read past TOK_END
  } else {
    error();
  }

  level = level - 1;
  if(printParse) {
    indent();
    cout << "Exit <compound>" << endl;
  }
  
  return newCompoundNode;
}
bool first_of_compound_statement() 
{
  return nextToken == TOK_BEGIN;
}

//*****************************************************************************
// Parses strings in the language generated by the rule:
// <if> → TOK_IF <expression> TOK_THEN <statement> [ TOK_ELSE <statement> ]
IfNode* if_statement() 
{
  if (nextToken != TOK_IF)
    error();

  if(printParse) {
    indent();
    cout << "Enter <if>" << endl;
  }
  level = level + 1;

  lex(); // Read past TOK_IF

  ExpressionNode* expr = expression();
  StatementNode* thenStatement = nullptr;
  StatementNode* elseStatement = nullptr;

  if (nextToken == TOK_THEN) {
    if(printParse) output();
    lex(); // Read past TOK_THEN
    thenStatement = statement();
  } else {
    error();
  }

  if (nextToken == TOK_ELSE) {
    if(printParse) output();
    lex(); // Read past TOK_ELSE
    elseStatement = statement();
  }

  IfNode* newIfNode = new IfNode(level, expr, thenStatement, elseStatement);

  level = level - 1;
  if(printParse) {
    indent();
    cout << "Exit <if>" << endl;
  }
  
  return newIfNode;
}

//*****************************************************************************
// Parses strings in the language generated by the rule:
// <while> → TOK_WHILE <expression> <statement>
WhileNode* while_statement() {
  if (nextToken != TOK_WHILE)
    error();

  if(printParse) {
    indent();
    cout << "Enter <while>" << endl;
  }
  level = level + 1;

  lex(); // Read past TOK_WHILE

  ExpressionNode* expr = expression();

  StatementNode* stmt = statement();

  WhileNode* newWhileNode = new WhileNode(level, expr, stmt);

  level = level - 1;
  if(printParse) {
    indent();
    cout << "Exit <while>" << endl;
  }
  
  return newWhileNode;
}

//*****************************************************************************
// Parses strings in the language generated by the rule:
// <read> → TOK_READ TOK_OPENPAREN TOK_IDENT TOK_CLOSEPAREN
ReadNode* read_statement() {
  if (nextToken != TOK_READ)
    error();

  if(printParse) {
    indent();
    cout << "Enter <read>" << endl;
  }
  level = level + 1;

  lex(); // Read past TOK_READ

  if (nextToken == TOK_OPENPAREN) {
    if(printParse) output();
    lex(); // Read past TOK_OPENPAREN
  } else {
    error();
  }

  std::string id;
  if (nextToken == TOK_IDENT) {
    if(printParse) output();
    id = std::string(yytext);
    lex(); // Read past the identifier
  } else {
    error();
  }

  if (nextToken == TOK_CLOSEPAREN) {
    if(printParse) output();
    lex(); // Read past TOK_CLOSEPAREN
  } else {
    error();
  }

  ReadNode* newReadNode = new ReadNode(level, id);

  level = level - 1;
  if(printParse) {
    indent();
    cout << "Exit <read>" << endl;
  }
  
  return newReadNode;
}

//*****************************************************************************
// Parses strings in the language generated by the rule:
// <write> → TOK_WRITE TOK_OPENPAREN ( TOK_IDENT | TOK_STRINGLIT ) TOK_CLOSEPAREN
WriteNode* write_statement() {
  if (nextToken != TOK_WRITE)
    error();

  if(printParse) {
    indent();
    cout << "Enter <write>" << endl;
  }
  level = level + 1;

  lex(); // Read past TOK_WRITE

  if (nextToken == TOK_OPENPAREN) {
    if(printParse) output();
    lex(); // Read past TOK_OPENPAREN
  } else {
    error();
  }

  string id;
  string str;

  if (nextToken == TOK_IDENT) {
    id = string(yytext);
    if(printParse) output();
    lex(); // Read past the identifier
  } else if (nextToken == TOK_STRINGLIT) {
    str = string(yytext);
    if(printParse) output();
    lex(); // Read past the string literal
  } else {
    error();
  }

  if (nextToken == TOK_CLOSEPAREN) {
    if(printParse) output();
    lex(); // Read past TOK_CLOSEPAREN
  } else {
    error();
  }

  WriteNode* newWriteNode = new WriteNode(level, id, str);

  level = level - 1;
  if(printParse) {
    indent();
    cout << "Exit <write>" << endl;
  }
  
  return newWriteNode;
}

//*****************************************************************************
// Parses strings in the language generated by the rule:
// <expression> → <simple_expression> [ ( TOK_EQUALTO | TOK_LESSTHAN | TOK_GREATERTHAN | TOK_NOTEQUALTO ) <simple_expression> ]
ExpressionNode* expression() {
  // Check that the <expr> starts with a valid token 
  if(!first_of_expression())
    error();

  if(printParse) {
    indent();
    cout << "Enter <expr>" << endl;
  }
  level = level + 1;
  ExpressionNode* newExprNode = new ExpressionNode(level);

  /* Parse the first term */
  newExprNode->firstSimpleExpr = simple_expression();

  if(nextToken == TOK_EQUALTO || nextToken == TOK_LESSTHAN || nextToken == TOK_GREATERTHAN || nextToken == TOK_NOTEQUALTO) {
    if(printParse) output();
    newExprNode->relop = nextToken;
    lex();
    newExprNode->secondSimpleExpr = simple_expression();
  }

  level = level - 1;
  if(printParse) {
    indent();
    cout << "Exit <expr>" << endl;
  }
  return newExprNode;
}
bool first_of_expression(void) 
{
  switch (nextToken) {
    case TOK_IDENT:
      return true;
    case TOK_INTLIT:
      return true;
    case TOK_FLOATLIT:
      return true;
    case TOK_OPENPAREN:
      return true;
    case TOK_NOT:
      return true;
    case TOK_MINUS:
      return true;
    default:
      return false;
  }
}

//*****************************************************************************
// Parses strings in the language generated by the rule:
// <simple_expression> → <term> { ( TOK_PLUS | TOK_MINUS | TOK_OR ) <term> }
SimpleExpressionNode* simple_expression() {
  if(!first_of_simple_expression())
    error();

  if(printParse) {
    indent();
    cout << "Enter <simple_expression>" << endl;
  }
  level = level + 1;
  SimpleExpressionNode* newSimpleExprNode = new SimpleExpressionNode(level);

  newSimpleExprNode->firstTerm = term();

  while (nextToken == TOK_PLUS || nextToken == TOK_MINUS || nextToken == TOK_OR) {
    if(printParse) output();
    newSimpleExprNode->restSmplExprOps.push_back(nextToken);
    lex();
    newSimpleExprNode->restTerms.push_back(term());
  }

  level = level - 1;
  if(printParse) {
    indent();
    cout << "Exit <simple_expression>" << endl;
  }
  return newSimpleExprNode;
}
bool first_of_simple_expression(void) 
{
  switch (nextToken) {
    case TOK_IDENT:
      return true;
    case TOK_INTLIT:
      return true;
    case TOK_FLOATLIT:
      return true;
    case TOK_OPENPAREN:
      return true;
    case TOK_NOT:
      return true;
    case TOK_MINUS:
      return true;
    default:
      return false;
  }
}

//*****************************************************************************
// Parses strings in the language generated by the rule:
// <term> → <factor> { ( TOK_MULTIPLY | TOK_DIVIDE | TOK_MOD ) <factor> }
TermNode* term() {
  /* Check that the <term> starts with a valid token */
  if(!first_of_term())
    error();

  if(printParse) {
    indent();
    cout << "Enter <term>" << endl;
  }
  level = level + 1;
  TermNode* newTermNode = new TermNode(level);

  /* Parse the first factor */
  newTermNode->firstFactor = factor();

  /* As long as the next token is * or /, get the
     next token and parse the next factor */
  while(nextToken == TOK_MULTIPLY || nextToken == TOK_DIVIDE || nextToken == TOK_MOD) {
    if(printParse) output();
    newTermNode->restTermOps.push_back(nextToken);
    lex();
    newTermNode->restFactors.push_back(factor());
  }

  level = level - 1;
  if(printParse) {
    indent();
    cout << "Exit <term>" << endl;
  }
  return newTermNode;
}
bool first_of_term(void) 
{
  switch (nextToken) {
    case TOK_IDENT:
      return true;
    case TOK_INTLIT:
      return true;
    case TOK_FLOATLIT:
      return true;
    case TOK_OPENPAREN:
      return true;
    case TOK_NOT:
      return true;
    case TOK_MINUS:
      return true;
    default:
      return false;
  }
}

//*****************************************************************************
// Parses strings in the language generated by the rule:
// <factor> → TOK_IDENT | TOK_INTLIT | TOK_FLOATLIT | TOK_OPENPAREN <expression> TOK_CLOSEPAREN | TOK_NOT <factor> | TOK_MINUS <factor>
FactorNode* factor() {
  // Check that the <factor> starts with a valid token
  if(!first_of_factor())
    error();

  if(printParse) {
  indent();
  cout << "Enter <factor>" << endl;
  }
  level = level + 1;
  FactorNode* newFactorNode = nullptr;

  // Determine which RHS token we have
  switch(nextToken) {

    case TOK_IDENT:
      if(printParse) output();
      newFactorNode = new IdentifierNode(level, string(yytext));
      nextToken = lex(); // Read past what we have found
      break;

    case TOK_INTLIT:
      if(printParse) output();
      newFactorNode = new IntLitNode(level, atoi(yytext));
      nextToken = lex();
      break;
    
    case TOK_FLOATLIT:
      if(printParse) output();
      newFactorNode = new FloatLitNode(level, atof(yytext));
      nextToken = lex();
      break;

    case TOK_OPENPAREN:
      // We expect ( <expr> ) parse it
      if(printParse) output();
      nextToken = lex();
      if (!first_of_expression()) 
        error();

      newFactorNode = new NestedExpressionNode(level, expression());

      if (nextToken == TOK_CLOSEPAREN) {
        if(printParse) output();
        nextToken = lex();
      }
      else
        error();
      break;
    
    case TOK_NOT:
      if(printParse) output();
      nextToken = lex();
      newFactorNode = new NotNode(level, factor());
      break;

    case TOK_MINUS:
      if(printParse) output();
      nextToken = lex();
      newFactorNode = new MinusNode(level, factor());
      break;

    default:
      // If we made it to here, syntax error
      error();
	}

  level = level - 1;
  
  if(printParse) {
    indent();
    cout << "Exit <factor>" << endl;
  }
  return newFactorNode;
}
bool first_of_factor(void) 
{
  switch (nextToken) {
    case TOK_IDENT:
      return true;
    case TOK_INTLIT:
      return true;
    case TOK_FLOATLIT:
      return true;
    case TOK_OPENPAREN:
      return true;
    case TOK_NOT:
      return true;
    case TOK_MINUS:
      return true;
    default:
      return false;
  }
}

