//*****************************************************************************
//  // purpose: Recursive descent parsing of arithmetic expressions
//  //          Builds the parse tree while parsing the input
//  // version: Spring 2025
//  // author: Delvin Buckley
//  //*****************************************************************************
#ifndef NODES_H
#define NODES_H

#include <iostream>
#include <vector>
#include <string>
#include "lexer.h"


using namespace std;

extern bool printDelete;      // shall we print deleting the tree?
extern bool printSymbolTable; // shall we print the symbol table?

// ---------------------------------------------------------------------
// Forward declaration of node types
class ProgramNode;
class BlockNode;
class StatementNode;
class AssignmentNode;
class CompoundNode;
class IfNode;
class WhileNode;
class ReadNode;
class WriteNode;
class ExpressionNode;
class SimpleExpressionNode;
class TermNode;
class FactorNode;
class IntLitNode;
class FloatLitNode;
class IdentifierNode;
class NestedExpressionNode;
class NotNode;
class MinusNode;

// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// <program> → TOK_PROGRAM TOK_IDENT TOK_SEMICOLON <block>
class ProgramNode {
public:
    int _level = 0; // recursion level of this node
    string* id = nullptr; // program name
    BlockNode* block = nullptr; // block of the program
    void interpret(); 
    ProgramNode(int level, string name, BlockNode* b);
    ~ProgramNode();
};
ostream& operator<<(ostream&, ProgramNode&); // Node print operator

// ---------------------------------------------------------------------
// <block> → ( TOK_VAR TOK_IDENT TOK_COLON type TOK_SEMIOLON {[ TOK_IDENT TOK_COLON type TOK_SEMIOLON ]} <compound> ) | <compound>
class BlockNode {
public:
    int _level = 0; // recursion level of this node
    /*vector<string*> varNames; // variable names
    vector<string*> varTypes; // variable types*/ 
    CompoundNode* compound = nullptr; // statement of the block
    void interpret();
    BlockNode(int level);
    ~BlockNode();
};
ostream& operator<<(ostream&, BlockNode&); // Node print operator

// ---------------------------------------------------------------------
// <statement> → <assignment> | <compound> | <if> | <while> | <read> | <write>
class StatementNode {
public:
  int _level = 0; // recursion level of this node
  virtual void interpret() = 0; 
  virtual void printTo(ostream &os) = 0; // method for abstract base class
  virtual ~StatementNode();
};
ostream& operator<<(ostream&, StatementNode&); // Node print operator

// ---------------------------------------------------------------------
// <assignment> → TOK_IDENT TOK_EQUALTO <expression>
//AssignmentNode(int level, string identifier, ExpressionNode* e)
class AssignmentNode : public StatementNode {
public:
    string* id = nullptr; // identifier name
    ExpressionNode* expr = nullptr; // expression to assign to the identifier
    void interpret();
    AssignmentNode(int level, string identifier, ExpressionNode* e);
    ~AssignmentNode();
    void printTo(ostream & os);
};

// ---------------------------------------------------------------------
// <compound> → TOK_BEGIN <statement> { TOK_SEMICOLON <statement> } TOK_END
class CompoundNode : public StatementNode {
public:
  vector<StatementNode*> statements; // vector of statements
  void interpret();
  CompoundNode(int level);
  ~CompoundNode();
  void addStatement(StatementNode* s); // add a statement to the vector
  void printTo(ostream & os);
};

// ---------------------------------------------------------------------
// <if> → TOK_IF <expression> TOK_THEN <statement> [ TOK_ELSE <statement> ]
class IfNode : public StatementNode {
public:
    ExpressionNode* expr = nullptr; // expression to evaluate
    StatementNode* thenStatement = nullptr; // statement to execute if expr == true
    StatementNode* elseStatement = nullptr; // statement to execute if expr == false
    void interpret();
    IfNode(int level, ExpressionNode* e, StatementNode* ts, StatementNode* es);
    ~IfNode();
    void printTo(ostream & os);
};

// ---------------------------------------------------------------------
// <while> → TOK_WHILE <expression> <statement>
class WhileNode : public StatementNode {
public:
    ExpressionNode* expr = nullptr; // expression to evaluate
    StatementNode* statement = nullptr; // statement to execute while expr == true
    void interpret();
    WhileNode(int level, ExpressionNode* e, StatementNode* s);
    ~WhileNode();
    void printTo(ostream & os);
};

// ---------------------------------------------------------------------
// <read> → TOK_READ TOK_OPENPAREN TOK_IDENT TOK_CLOSEPAREN
class ReadNode : public StatementNode {
public:
    string* id = nullptr; // identifier name
    void interpret();
    ReadNode(int level, string name);
    ~ReadNode();
    void printTo(ostream & os);
};

// ---------------------------------------------------------------------
// <write> → TOK_WRITE TOK_OPENPAREN ( TOK_IDENT | TOK_STRINGLIT ) TOK_CLOSEPAREN
class WriteNode : public StatementNode {
public:
  string* id = nullptr; // identifier name
  string* str = nullptr; // string literal
  void interpret();
  WriteNode(int level, string name, string str);
  ~WriteNode();
  void printTo(ostream & os);
};

// ---------------------------------------------------------------------
// <expression> → <simple_expression> [ ( TOK_EQUALTO | TOK_LESSTHAN | TOK_GREATERTHAN | TOK_NOTEQUALTO ) <simple_expression> ]
class ExpressionNode {
public:
  int _level = 0; // recursion level of this node
  int relop = 0; // TOK_EQUALTO, TOK_LESSTHAN, TOK_GREATERTHAN, or TOK_NOTEQUALTO
  SimpleExpressionNode* firstSimpleExpr = nullptr; // first simple expression
  SimpleExpressionNode* secondSimpleExpr = nullptr; // second simple expression
  float interpret();
  ExpressionNode(int level);
  ~ExpressionNode();
};
ostream& operator<<(ostream&, ExpressionNode&); // Node print operator

// ---------------------------------------------------------------------
// <simple_expression> → <term> { ( TOK_PLUS | TOK_MINUS | TOK_OR ) <term> }
class SimpleExpressionNode {
public:
  int _level = 0; // recursion level of this node
  TermNode* firstTerm = nullptr; // first term
  vector<int> restSmplExprOps; // vector of TOK_ADD, TOK_MINUS, or TOK_OR operators
  vector<TermNode*> restTerms; // vector of terms
  float interpret();
  SimpleExpressionNode(int level);
  ~SimpleExpressionNode();
};
ostream& operator<<(ostream&, SimpleExpressionNode&); // Node print operator


// ---------------------------------------------------------------------
// <term> → <factor> { ( TOK_MULTIPLY | TOK_DIVIDE | TOK_MOD ) <factor> }
class TermNode {
public:
  int _level = 0; // recursion level of this node
  FactorNode* firstFactor = nullptr; // first factor
  vector<int> restTermOps; // vector of TOK_MULTIPLY, TOK_DIVIDE, or TOK_MOD operators
  vector<FactorNode*> restFactors; // vector of factors
  float interpret();
  TermNode(int level);
  ~TermNode();
};
ostream& operator<<(ostream&, TermNode&); // Node print operator

// ---------------------------------------------------------------------
// <factor> → TOK_IDENT | TOK_INTLIT | TOK_FLOATLIT | TOK_OPENPAREN <expression> TOK_CLOSEPAREN | TOK_NOT <factor> | TOK_MINUS <factor>
class FactorNode {
public:
  int _level = 0; // recursion level of this node
  virtual float interpret() = 0;
  virtual void printTo(ostream &os) = 0; // pure virtual method, makes the class Abstract
  virtual ~FactorNode();
};
ostream& operator<<(ostream&, FactorNode&); // Node print operator

class IntLitNode : public FactorNode {
public:
    float int_literal = 0; // integer literal value
    float interpret();
    IntLitNode(int level, int value);
    ~IntLitNode();
    void printTo(ostream & os);
};

class FloatLitNode : public FactorNode {
public:
    float float_literal = 0.0; // float literal value
    float interpret();
    FloatLitNode(int level, float value);
    ~FloatLitNode();
    void printTo(ostream & os);
};

class IdentifierNode : public FactorNode {
public:
    string* id = nullptr; // identifier name
    float interpret();
    IdentifierNode(int level, string name);
    ~IdentifierNode();
    void printTo(ostream & os);
};

class NestedExpressionNode : public FactorNode {
public:
    ExpressionNode* exprPtr = nullptr; // pointer to the expression
    float interpret();
    NestedExpressionNode(int level, ExpressionNode* en);
    ~NestedExpressionNode();
    void printTo(ostream & os);
};

class NotNode : public FactorNode {
public:
    FactorNode* factor; // pointer to the factor
    float interpret();
    NotNode(int level, FactorNode* f);
    ~NotNode();
    void printTo(ostream & os);
};

class MinusNode : public FactorNode {
public:
    FactorNode* factor; // pointer to the factor
    float interpret();
    MinusNode(int level, FactorNode* f);
    ~MinusNode();
    void printTo(ostream & os);
};

#endif /* NODES_H */
