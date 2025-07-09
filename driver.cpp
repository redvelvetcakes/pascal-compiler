//*****************************************************************************
// purpose: Recursive descent parsing of arithmetic expressions
//          Builds the parse tree while parsing the input
// version: Spring 2025
//  author: Delvin Buckley
//*****************************************************************************
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <cstring>
#include "lexer.h"
#include "parser.h"
#include "nodes.h"

using namespace std;

extern "C"
{
  // Instantiate global variables
  extern FILE *yyin;   // input stream
  extern FILE *yyout;  // output stream
  extern int yyleng;   // length of current lexeme
  extern char *yytext; // text of current lexeme
  extern int yylex();  // the generated lexical analyzer
  extern int yylex_destroy(); // deletes memory allocated by yylex
}

extern bool printParse;       // shall tree be printed while parsing?
extern bool printTree;        // shall we print the tree?

int main( int argc, char* argv[] )
{
  // Whether to print these items
  bool printDelete = false;      // shall we print deleting the tree?
  bool printSymbolTable = false; // shall we print the symbol table?
  // Process any command-line switches
  for(int i = 1; i < argc; i++) {
    // -p flag: if requested, print while parsing
    if(std::strcmp(argv[i], "-p") == 0) {
      printParse = true;
    }
    // -t flag: if requested, print parse tree
    if(std::strcmp(argv[i], "-t") == 0) {
      printTree = true;
    }
    // -s flag: if requested, print symbol table
    if(std::strcmp(argv[i], "-s") == 0) {
      printSymbolTable = true;
    }
    // -d flag: if requested, print while deleting parse tree
    if(std::strcmp(argv[i], "-d") == 0) {
      printDelete = true;
    }
  }

  if (argc > 1) {
    // If a file name is provided, open it
    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
      cout << "ERROR - cannot open " << argv[1] << endl;
      return(EXIT_FAILURE);
    }
  }

  // Create the root of the parse tree
  ProgramNode* root = nullptr;

  lex();  // prime the pump (get first token)

  root = program(); // start symbol is <expr>

  if (yyin)
    fclose(yyin);
    
  yylex_destroy();

  // Printing, Interpreting, and Deleting the tree all result in 
  // the same in-order traversal of the tree as parsing.  All
  // use the call stack.

  if(printTree) {
    cout << endl << "*** Print the Tree ***" << endl;
    cout << *root << endl << endl;
  }

  cout << "*** Interpret the Tree ***" << endl;
  root->interpret();
  cout << endl;

  if(printSymbolTable)
  {
    cout << "*** Print the Symbol Table ***" << endl;
    symbolTableT::iterator it;
    for(it = symbolTable.begin(); it != symbolTable.end(); ++it )
      cout << setw(8) << it->first << ": " << it->second << endl;
  }
  
  if(printDelete)
    cout << "*** Delete the Tree ***" << endl;
  delete root;
  root = nullptr;
    
  return(EXIT_SUCCESS);
}
