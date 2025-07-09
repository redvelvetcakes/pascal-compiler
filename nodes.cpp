//*****************************************************************************
//  // purpose: Recursive descent parsing of arithmetic expressions
//  //          Builds the parse tree while parsing the input
//  // version: Spring 2025
//  // author: Delvin Buckley
//  //*****************************************************************************


#include "nodes.h"
#include "parser.h"

#define EPSILON 0.001
// Define truth for a floating-point number:
// falsehood == F is within EPSILON of 0.0
// truth == not falsehood
static bool truth(float F) {
  return !((EPSILON > F) && (F > -EPSILON));
}


bool printDelete = false;   // shall we print deleting the tree?

// ---------------------------------------------------------------------
// Indent according to tree level
static void indent(int level) {
  for (int i = 0; i < level; i++)
    cout << ("| ");
}

// ---------------------------------------------------------------------
ProgramNode::ProgramNode(int level, string name, BlockNode* b) {
  _level = level;
  id = new string(name);
  block = b;
}
ProgramNode::~ProgramNode() {
  if(printDelete) 
    cout << "Deleting ProgramNode " << endl;
  delete id;
  id = nullptr;
  delete block;
  block = nullptr;
}
ostream& operator<<(ostream& os, ProgramNode& pn) {
  os << endl; indent(pn._level); os << "(program ";
  os << *(pn.id);
  os << *(pn.block);
  os << endl; indent(pn._level); os << "program) ";
  return os;
}
void ProgramNode::interpret() {
  block->interpret();
}

// ---------------------------------------------------------------------
BlockNode::BlockNode(int level) {
  _level = level;
}
BlockNode::~BlockNode() {
  if(printDelete) 
    cout << "Deleting BlockNode " << endl;
  /*int nameLength = varNames.size();
  for (int i = 0; i < nameLength; ++i) {
    delete varNames[i];
    varNames[i] = nullptr;
  }
  int typeLength = varTypes.size();
  for (int i = 0; i < typeLength; ++i) {
    delete varTypes[i];
    varTypes[i] = nullptr;
  } // Not needed, as the symbol table is already initialized in the parser and does not store types.*/
  delete compound;
  compound = nullptr;
}
ostream& operator<<(ostream& os, BlockNode& bn) {
  os << endl; indent(bn._level); os << "(block ";
  indent(bn._level);
  os << *(bn.compound);
  os << endl; indent(bn._level); os << "block) ";
  return os;
}
void BlockNode::interpret() {
  // Initialize the symbol table with variable names and types
  /*for (int i = 0; i < varNames.size(); ++i) {
    std::string name = *(varNames[i]);
    std::string type = *(varTypes[i]);
    if (type == "INTEGER") {
      symbolTable[name] = 0; // Initialize integer variable to 0
    } else if (type == "REAL") {
      symbolTable[name] = 0.0; // Initialize real variable to 0.0
    }
  } Not needed, as the symbol table is already initialized in the parser and does not store types.*/
  if (compound != nullptr) {
    compound->interpret();
  }
}

// ---------------------------------------------------------------------
StatementNode::~StatementNode() {
  if(printDelete) 
    cout << "Deleting StatementNode " << endl;
}
ostream& operator<<(ostream& os, StatementNode& sn) {
  sn.printTo(os);
  return os;
}
void StatementNode::interpret() {
  return this->interpret();
}

// ---------------------------------------------------------------------
AssignmentNode::AssignmentNode(int level, string identifier, ExpressionNode* e) {
  _level = level;
  id = new string(identifier);
  expr = e;
}
AssignmentNode::~AssignmentNode() {
  if(printDelete) 
    cout << "Deleting AssignmentNode " << endl;
  delete id;
  id = nullptr;
  delete expr;
  expr = nullptr;
}
void AssignmentNode::printTo(ostream & os) {
  os << endl; indent(_level); os << "(assignment ";
  os << "( " << *id << " := )";
  os << *expr;
  os << endl; indent(_level); os << "assignment) ";
} 
void AssignmentNode::interpret() {
  symbolTableT::iterator variable = symbolTable.find(*id); // Look up the variable that will store expression result
  variable->second = expr->interpret(); // Put the expression in the variable
}

// ---------------------------------------------------------------------
CompoundNode::CompoundNode(int level) {
  _level = level;
}
CompoundNode::~CompoundNode() {
  if(printDelete) 
    cout << "Deleting CompoundNode " << endl;
  for (int i = 0; i < statements.size(); ++i) {
    delete statements[i];
    statements[i] = nullptr;
  }
}
void CompoundNode::addStatement(StatementNode* s) {
  statements.push_back(s);
}
void CompoundNode::printTo(ostream & os) {
  os << endl; indent(_level); os << "(compound_stmt";
  for (int i = 0; i < statements.size(); ++i) {
    os << *statements[i];
  }
  os << endl; indent(_level); os << "compound_stmt)";
}
void CompoundNode::interpret() {
  for (int i = 0; i < statements.size(); ++i) {
    statements[i]->interpret();
  }
}

// ---------------------------------------------------------------------
IfNode::IfNode(int level, ExpressionNode* e, StatementNode* ts, StatementNode* es) {
  _level = level;
  expr = e;
  thenStatement = ts;
  elseStatement = es;
}
IfNode::~IfNode() {
  if(printDelete) 
    cout << "Deleting IfNode " << endl;
  delete expr;
  expr = nullptr;
  delete thenStatement;
  thenStatement = nullptr;
  delete elseStatement;
  elseStatement = nullptr;
}
void IfNode::printTo(ostream & os) {
  os << endl; indent(_level); os << "(if_stmt ";
  os << *expr;
  if (thenStatement) {
    os << endl; indent(_level); os << "(then ";
    os << *thenStatement;
    os << endl; indent(_level); os << "then)";
  }
  if (elseStatement) {
    os << endl; indent(_level); os << "(else ";
    os << *elseStatement;
    os << endl; indent(_level); os << "else)";
  }
  os << endl; indent(_level); os << "if_stmt)";
}
void IfNode::interpret() {
  if (truth(expr->interpret())) {
    thenStatement->interpret();
  } else if (elseStatement) {
    elseStatement->interpret();
  }
}

// ---------------------------------------------------------------------
WhileNode::WhileNode(int level, ExpressionNode* e, StatementNode* s) {
  _level = level;
  expr = e;
  statement = s;
}
WhileNode::~WhileNode() {
  if(printDelete) 
    cout << "Deleting WhileNode " << endl;
  delete expr;
  expr = nullptr;
  delete statement;
  statement = nullptr;
}
void WhileNode::printTo(ostream & os) {
  os << endl; indent(_level); os << "(while ";
  os << *expr;
  os << *statement;
  os << endl; indent(_level); os << "while)";
}
void WhileNode::interpret() {
  while (truth(expr->interpret())) {
    statement->interpret();
  }
}

// ---------------------------------------------------------------------
ReadNode::ReadNode(int level, string name) {
  _level = level;
  id = new string(name);
}
ReadNode::~ReadNode() {
  if(printDelete) 
    cout << "Deleting ReadNode " << endl;
  delete id;
  id = nullptr;
}
void ReadNode::printTo(ostream & os) {
  os << endl; indent(_level); os << "(read_stmt ( ";
  os << *id; os << " )";
  os << endl; indent(_level); os << "read_stmt)";
}
void ReadNode::interpret() {
  // Read a value from the user and store it in the variable
  float value;
  cout << "Enter value for " << *id << ": ";
  cin >> value;
  symbolTable[*id] = value; // Store the value in the symbol table
}

// ---------------------------------------------------------------------
WriteNode::WriteNode(int level, string name, string str) {
  _level = level;
  if (!name.empty()) id = new string(name);
  if (!str.empty()) this->str = new string(str);
}
WriteNode::~WriteNode() {
  if(printDelete) 
    cout << "Deleting WriteNode " << endl;
  delete id;
  id = nullptr;
  delete str;
  str = nullptr;
}
void WriteNode::printTo(ostream & os) {
  os << endl; indent(_level); os << "(write_stmt ( ";
  if (id) {
    os << *id; os << " )";
  } else if (str) {
    os << *str; os << " )";
  }
  os << endl; indent(_level); os << "write_stmt)";
}
/*void WriteNode::interpret() {
  if (id) {
    // Look up the variable in the symbol table and print its value
    symbolTableT::iterator variable = symbolTable.find(id->c_str());
    if (variable != symbolTable.end()) {
      cout << variable->first << " = " << variable->second << endl;
    } else {
      cout << "Variable " << *id << " not found." << endl;
    }
  } else if (str) {
    // Print the string literal
    cout << *str << endl;
  }
}*/
void WriteNode::interpret() {
  if (id) {
    auto variable = symbolTable.find(*id);
    cout << variable->second << endl;
  } else if (str) {
    // Print the string literal
    cout << str->substr(1, str->length() - 2) << endl;
  }
}

// ---------------------------------------------------------------------
ExpressionNode::ExpressionNode(int level) {
  _level = level;
}
ExpressionNode::~ExpressionNode() {
  if(printDelete)
    cout << "Deleting ExprNode " << endl;
	delete firstSimpleExpr;
	firstSimpleExpr = nullptr;
  delete secondSimpleExpr;
  secondSimpleExpr = nullptr;
}
ostream& operator<<(ostream& os, ExpressionNode& en) {
  os << endl; indent(en._level); os << "(expression ";
  os << *(en.firstSimpleExpr);
  switch (en.relop) {
    case TOK_EQUALTO:
      os << endl; indent(en._level); os << "= ";
      os << *(en.secondSimpleExpr);
      break;
    case TOK_LESSTHAN:
      os << endl; indent(en._level); os << "< ";
      os << *(en.secondSimpleExpr);
      break;
    case TOK_GREATERTHAN:
      os << endl; indent(en._level); os << "> ";
      os << *(en.secondSimpleExpr);
      break;
    case TOK_NOTEQUALTO:
      os << endl; indent(en._level); os << "<> ";
      os << *(en.secondSimpleExpr);
      break;
    default:
      break;
  }
  os << endl; indent(en._level); os << "expression) ";
  return os;
}
float ExpressionNode::interpret() {
  float value = firstSimpleExpr->interpret();
  if (relop != 0) {
    float secondValue = secondSimpleExpr->interpret();
    switch (relop) {
      case TOK_EQUALTO:
        return truth(value - secondValue) ? 1.0f : 0.0f;
      case TOK_LESSTHAN:
        return value < secondValue ? 1.0f : 0.0f;
      case TOK_GREATERTHAN:
        return value > secondValue ? 1.0f : 0.0f;
      case TOK_NOTEQUALTO:
        return truth(value - secondValue) ? 0.0f : 1.0f;
      default:
        break;
    }
  }
  return value;
}

// ---------------------------------------------------------------------
SimpleExpressionNode::SimpleExpressionNode(int level) {
  _level = level;
}
SimpleExpressionNode::~SimpleExpressionNode() {
  if(printDelete) 
    cout << "Deleting SimpleExpressionNode " << endl;
  delete firstTerm;
  firstTerm = nullptr;

  int length = restTerms.size();
  for (int i = 0; i < length; ++i) {
    delete restTerms[i];
    restTerms[i] = nullptr;
  }
}
ostream & operator<<(ostream& os, SimpleExpressionNode& sen) {
  os << endl; indent(sen._level); os << "(simple_exp ";
  os << *(sen.firstTerm);

  int length = sen.restSmplExprOps.size();
  for (int i = 0; i < length; ++i) {
    int op = sen.restSmplExprOps[i];
    switch (op) {
      case TOK_PLUS:
        os << endl; indent(sen._level); os << "+ ";
        break;
      case TOK_MINUS:
        os << endl; indent(sen._level); os << "- ";
        break;
      case TOK_OR:
        os << endl; indent(sen._level); os << "OR ";
        break;
      default:
        break;
    }
    os << *(sen.restTerms[i]);
  }
  os << endl; indent(sen._level); os << "simple_expr) ";
  return os;
}
float SimpleExpressionNode::interpret() {
  float value = firstTerm->interpret();
  int length = restTerms.size();
  for (int i = 0; i < length; ++i) {
    int op = restSmplExprOps[i];
    float nextValue = restTerms[i]->interpret();
    switch (op) {
      case TOK_PLUS:
        value += nextValue;
        break;
      case TOK_MINUS:
        value -= nextValue;
        break;
      case TOK_OR:
        value = truth(value) || truth(nextValue) ? 1.0f : 0.0f; // Return 1.0 if either value is true, otherwise return 0.0
        break;
      default:
        break;
    }
  }
  return value;
}

// ---------------------------------------------------------------------
TermNode::TermNode(int level) {
  _level = level;
}
TermNode::~TermNode() {
  if(printDelete) 
    cout << "Deleting TermNode " << endl;
  delete firstFactor;
  firstFactor = nullptr;

  int length = restFactors.size();
  for (int i = 0; i < length; ++i) {
    delete restFactors[i];
    restFactors[i] = nullptr;
  }
}
ostream& operator<<(ostream& os, TermNode& tn) {
  os << endl; indent(tn._level); os << "(term ";
  os << *(tn.firstFactor);

  int length = tn.restTermOps.size();
  for (int i = 0; i < length; ++i) {
    int op = tn.restTermOps[i];
    switch (op) {
      case TOK_MULTIPLY:
        os << endl; indent(tn._level); os << "* ";
        break;
      case TOK_DIVIDE:
        os << endl; indent(tn._level); os << "/ ";
        break;
      case TOK_AND:
        os << endl; indent(tn._level); os << "AND ";
        break;
      default:
        break;
    }
    os << *(tn.restFactors[i]);
  }
  os << endl; indent(tn._level); os << "term) ";
  return os;
}
float TermNode::interpret() {
  float value = firstFactor->interpret();
  int length = restFactors.size();
  for (int i = 0; i < length; ++i) {
    int op = restTermOps[i];
    float nextValue = restFactors[i]->interpret();
    switch (op) {
      case TOK_MULTIPLY:
        value *= nextValue;
        break;
      case TOK_DIVIDE:
        value /= nextValue;
        break;
      case TOK_MOD:
        value = static_cast<int>(value) % static_cast<int>(nextValue);
        break;
      default:
        break;
    }
  }
  return value;
}

// ---------------------------------------------------------------------
FactorNode::~FactorNode() {
  if(printDelete) 
    cout << "Deleting FactorNode " << endl;
}
ostream& operator<<(ostream& os, FactorNode& fn) {
  os << endl; indent(fn._level); os << "(factor ";
  fn.printTo(os);
  os << endl; indent(fn._level); os << "factor) ";
  return os;
}

// ---------------------------------------------------------------------
IntLitNode::IntLitNode(int level, int value) {
  _level = level;
  int_literal = static_cast<float>(value);
}
IntLitNode::~IntLitNode() {
  if(printDelete) 
    cout << "Deleting IntLitNode " << endl;
}
void IntLitNode::printTo(ostream& os) {
  os << "(INTLIT: " << int_literal << ") ";
}
float IntLitNode::interpret() {
  return int_literal;   
}

// ---------------------------------------------------------------------
FloatLitNode::FloatLitNode(int level, float value) {
  _level = level;
  float_literal = value;
}
FloatLitNode::~FloatLitNode() {
  if(printDelete) 
    cout << "Deleting FloatLitNode " << endl;
}
void FloatLitNode::printTo(ostream& os) {
  os << "(FLOATLIT: " << float_literal << ") ";
}
float FloatLitNode::interpret() {
  return float_literal;   
}

// ---------------------------------------------------------------------
IdentifierNode::IdentifierNode(int level, string name) {
  _level = level;
  id = new string(name);
}
IdentifierNode::~IdentifierNode() {
  if(printDelete) 
    cout << "Deleting IdentifierNode " << endl;
  delete id;
  id = nullptr;
}
void IdentifierNode::printTo(ostream& os) {
  os << "( IDENT: " << *id << " ) ";
}
float IdentifierNode::interpret() {
  return symbolTable[*id];
}

// ---------------------------------------------------------------------
NestedExpressionNode::NestedExpressionNode(int level, ExpressionNode* en) {
  _level = level;
  exprPtr = en;
}
NestedExpressionNode::~NestedExpressionNode() {
  if(printDelete) 
    cout << "Deleting NestedExpressionNode " << endl;
  delete exprPtr;
  exprPtr = nullptr;
}
void NestedExpressionNode::printTo(ostream& os) {
  os << "(NESTED_EXPR: ";
  os << *exprPtr;
  os << ") ";
}
float NestedExpressionNode::interpret() {
  return exprPtr->interpret();
}

// ---------------------------------------------------------------------
NotNode::NotNode(int level, FactorNode* f) {
  _level = level;
  factor = f;
}
NotNode::~NotNode() {
  if(printDelete) 
    cout << "Deleting NotNode " << endl;
  delete factor;
  factor = nullptr;
}
void NotNode::printTo(ostream& os) {
  os << "(NOT: ";
  os << *factor;
  os << ") ";
}
float NotNode::interpret() {
  float value = factor->interpret();
  return truth(value) ? 0.0f : 1.0f; // Return 1.0 if the value is false, otherwise return 0.0
}

// ---------------------------------------------------------------------
MinusNode::MinusNode(int level, FactorNode* f) {
  _level = level;
  factor = f;
}
MinusNode::~MinusNode() {
  if(printDelete) 
    cout << "Deleting MinusNode " << endl;
  delete factor;
  factor = nullptr;
}
void MinusNode::printTo(ostream& os) {
  os << "(-: ";
  os << *factor;
  os << ") ";
}
float MinusNode::interpret() {
  return static_cast<float>(-factor->interpret()); // Negate the value of the factor
}
