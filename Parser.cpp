#include "Par.h"
#define BLANK -999999999

int memoryAddr = 7000;
bool isFromRead = false;
int countSymbol = 0;
bool isFromDeclaration = false;
string prevLexeme;
string temp;
bool beenRead = false;

// Constructor
Parser::Parser() {
  symbolIndex = 0;
  symbolTable[symbolIndex].memoryLocation = 0;
  instructionIndex = 1;
}

// Print token and lexeme helper function
void Parser::print(ofstream &outfile) {
  Lexer::print(); // Call the print function of the Lexer class.
  outfile << "Token: " << left << setw(20) << this->token
          << "Lexeme: " << setw(20) << this->lexeme << endl;
}

// Parse symbol table
bool Parser::CheckSymbol(string lexeme) {
  for (int i = 0; i < symbolIndex; i++) {
    if (lexeme == symbolTable[i].id) {
      if (isFromDeclaration == true)
        countSymbol++;
      return true;
    }
  }
  return false;
}

// Add symbol to table
void Parser::GenerateSymbol(string lexeme, string idType) {
  symbolTable[symbolIndex].id = lexeme;
  symbolTable[symbolIndex].memoryLocation = memoryAddr;
  symbolTable[symbolIndex].idType = idType;
  symbolIndex++;
  memoryAddr++;
}

// Print symbol table
void Parser::PrintSymbol(ifstream &infile, ostream &outfile) {
  cout << "\nSYMBOL TABLE\n";
  outfile << "\nSYMBOL TABLE\n";
  for (int i = 0; i < symbolIndex; i++) {
    cout << symbolTable[i].id << " " << symbolTable[i].memoryLocation << " "
         << symbolTable[i].idType;
    cout << endl;
    outfile << symbolTable[i].id << " " << symbolTable[i].memoryLocation << " "
            << symbolTable[i].idType;
    outfile << endl;
  }
}

// Add instruction to table
void Parser::GenerateInstruction(string op, int oprnd) {
  instructionTable[instructionIndex].address = instructionIndex;
  instructionTable[instructionIndex].op = op;
  instructionTable[instructionIndex].oprnd = oprnd;
  instructionIndex++;
}

int Parser::GetAddress(string temp) const {
  int address = 0;
  for (int i = 0; i < symbolIndex; i++) {
    if (symbolTable[i].id == temp) {
      address = symbolTable[i].memoryLocation;
    }
  }
  return address;
}

// Print assembly code
void Parser::PrintInstruction(ifstream &infile, ostream &outfile) {
  string a;
  cout << "\n"
       << "Instruction Table"
       << "\n";
  outfile << "\n"
          << "Instruction Table"
          << "\n";
  for (int i = 1; i < instructionIndex; i++) {
    if (instructionTable[i].oprnd == BLANK) {
      a = "";
      cout << instructionTable[i].address << " " << instructionTable[i].op
           << " " << a << endl;
      outfile << instructionTable[i].address << " " << instructionTable[i].op
              << " " << a << endl;
    } else {
      cout << instructionTable[i].address << " " << instructionTable[i].op
           << " " << instructionTable[i].oprnd << endl;
      outfile << instructionTable[i].address << " " << instructionTable[i].op
              << " " << instructionTable[i].oprnd << endl;
    }
  }
}

// Get top of stack
void Parser::BackPatch(int address) {
  int addr = jumpStack.top();
  jumpStack.pop();
  instructionTable[addr].oprnd = address;
}

// Return token type
string Parser::GetType(string input) const {
  string a = "";
  for (int i = 0; i < symbolIndex; i++) {
    if (symbolTable[i].id == input) {
      a = symbolTable[i].idType;
    }
  }
  if (input == "true" || input == "false") {
    a = "boolean";
  }
  return a;
}

// Double checks that there are no boolean operations
void Parser::CheckTypeMatch(string preLexeme, string lexeme, ifstream &infile,
                            ostream &outfile) {
  if (GetType(prevLexeme) == "boolean" || GetType(lexeme) == "boolean") {
    outfile << "No Arithmetic Operations are allowed for boolean." << endl;
    cerr << "No Arithmetic Operations are allowed for boolean." << endl;

    exit(1);
  }
}

// Syntax rule functions follow:

// R1. <Rat23F> ::= <Opt Function Definitions> # <Opt Declaration List>
// <StatementList> # This function parses the RAT23F grammar rule.
void Parser::RAT23F(ifstream &infile, ofstream &outfile) {
  // Get the first token from the input file.
  lexer(infile);

  // Parse the optional function definitions, declaration list, and
  // statement list. OptFunctionDefinitions(infile, outfile);
  if (lexeme == "#") {
    lexer(infile);

    OptDeclarationList(infile, outfile);
    StatementList(infile, outfile);
    if (!(lexeme == "#")) {
      exit(1);
    }

    // Reset the line number after finishing syntax checking for a file.
    lineNum = 1;
  } else {
    // Error handling for invalid separator.
    printError(outfile);
    outfile << "Invalid separator, '#' is expected "
            << " after function definitions and before declaration list.\n";
    cerr << "Invalid separator, '#' is expected "
         << " after function definitions and before declaration list.\n";

    exit(1);
  }
}

// R10. <Qualifier> ::= integer | bool | real
void Parser::Qualifier(ifstream &infile, ofstream &outfile) {
  if (token == "keyword" && lexeme == "integer") {

    currentType = lexeme;

    lexer(infile);

  } else if (token == "keyword" && lexeme == "bool") {

    currentType = lexeme;

    lexer(infile);

  } else {
    printError(outfile);
    outfile << "Qualifier syntax error\n"
            << "Invalid keyword\n";
    outfile << "'integer', 'bool' or 'real' keyword is expected"
            << " after identifier.\n";
    cerr << "Qualifier syntax error\n"
         << "Invalid keyword\n";
    cerr << "'integer', 'bool' or 'real' keyword is expected"
         << " after identifier.\n";

    exit(1);
  }
}

// R11. <Body> ::= { <Statement List> }
void Parser::Body(ifstream &infile, ofstream &outfile) {
  if (lexeme == "{") {

    lexer(infile);

    StatementList(infile, outfile);
    if (lexeme == "}") {
      lexer(infile);

    } else {
      printError(outfile);
      outfile << "Function Body syntax error\n";
      outfile << "Invalid separator, '}' is expected"
              << " after statements.\n";
      cerr << "Function Body syntax error\n";
      cerr << "Invalid separator, '}' is expected"
           << " after statements.\n";

      exit(1);
    }
  } else {
    printError(outfile);
    outfile << "Function Body syntax error";
    outfile << "Invalid separator, '{' is expected"
            << " before any statements.\n";
    cerr << "Function Body syntax error";
    cerr << "Invalid separator, '{' is expected"
         << " before any statements.\n";

    exit(1);
  }
}

// R12. <Opt Declaration List> ::= <Declaration List> | <Empty>
void Parser::OptDeclarationList(ifstream &infile, ofstream &outfile) {
  if (lexeme == "integer" || lexeme == "bool" /*|| lexeme == "real"*/) {

    DeclarationList(infile, outfile);
  } else {
    Empty(infile, outfile);
  }
}

// R13. <Declaration List> ::= <Declaration> ; <Declaration List'>
void Parser::DeclarationList(ifstream &infile, ofstream &outfile) {
  isFromDeclaration = true;

  Declaration(infile, outfile);
  if (lexeme == ";") {
    lexer(infile);

    DeclarationListPrime(infile, outfile);
  } else {
    printError(outfile);
    outfile << "Declaration List syntax error\n";
    outfile << "Invalid separator, ';' is expected"
            << " at the end of declaration.\n";
    cerr << "Declaration List syntax error\n";
    cerr << "Invalid separator, ';' is expected"
         << " at the end of declaration.\n";

    exit(1);
  }
}

// R14. <Declaration List'> ::= <Declaration List> | <Empty>
void Parser::DeclarationListPrime(ifstream &infile, ofstream &outfile) {
  if (lexeme == "integer" || lexeme == "bool" || lexeme == "real") {

    DeclarationList(infile, outfile);
  } else {

    Empty(infile, outfile);
  }
}

// R15. <Declaration> ::= <Qualifier> <IDs>
void Parser::Declaration(ifstream &infile, ofstream &outfile) {

  Qualifier(infile, outfile);
  IDs(infile, outfile);
}

// R16. <IDs> ::= <Identifier> <IDs'>
void Parser::IDs(ifstream &infile, ofstream &outfile) {
  if (token == "identifier") {

    if (!CheckSymbol(lexeme) && beenRead == false) {

      GenerateSymbol(lexeme, currentType);

    } else if (!CheckSymbol(lexeme) && beenRead == true) {

      outfile << "no identifier! lexeme: " << lexeme << endl;
      cout << "no identifier! lexeme: " << lexeme << endl;

      exit(1);
    }

    if (countSymbol == 2) {
      outfile << "already declared! lexeme: " << lexeme << endl;
      cout << "already declared! lexeme: " << lexeme << endl;
    }

    lexer(infile);

    IDsPrime(infile, outfile);
  } else {
    printError(outfile);
    outfile << "IDs syntax error\n";
    outfile << "Invalid token, <identifier> is expected"
               " after '(' or ','.\n";
    cerr << "IDs syntax error\n";
    cerr << "Invalid token, <identifier> is expected"
            " after '(' or ','.\n";

    exit(1);
  }
}

// R17. <IDs'> ::= , <IDs> | <Empty>
void Parser::IDsPrime(ifstream &infile, ofstream &outfile) {
  if (lexeme == ",") {

    lexer(infile);

    IDs(infile, outfile);
  } else {
    Empty(infile, outfile);
  }
}

// R18. <Statement List> ::= <Statement> <Statement List'>
void Parser::StatementList(ifstream &infile, ofstream &outfile) {
  Statement(infile, outfile);
  StatementListPrime(infile, outfile);
}

// R19. <Statement List'> ::= <Statement List> | <Empty>
void Parser::StatementListPrime(ifstream &infile, ofstream &outfile) {
  if (lexeme == "{" || token == "identifier" || lexeme == "ret" ||
      lexeme == "put" || lexeme == "put" || lexeme == "get" ||
      lexeme == "while") {
    StatementList(infile, outfile);
  } else {
    if (!(lexeme == "EOF")) {
      ;
    }
    Empty(infile, outfile);
  }
}

// R20. <Statement> ::= <Compound> | <Assign> | <If> | <Return> | <Print> |
// <Scan> | <While>
void Parser::Statement(ifstream &infile, ofstream &outfile) {
  if (lexeme == "{") {
    Compound(infile, outfile);
  } else if (token == "identifier") {
    Assign(infile, outfile);
  } else if (lexeme == "if") {
    If(infile, outfile);
  } else if (lexeme == "ret") {
    Return(infile, outfile);
  } else if (lexeme == "put") {
    Print(infile, outfile);
  } else if (lexeme == "get") {
    Scan(infile, outfile);
  } else if (lexeme == "while") {

    While(infile, outfile);
  } else {
    printError(outfile);
    outfile << "Statement syntax error\n";
    outfile << "Invalid token or keyword or separator\n";
    outfile << "<identifier>,'if', 'ret', 'put', 'get','while' "
            << "keyword or '{'is expected at the beginning of a statement.\n";
    cerr << "Statement syntax error\n";
    cerr << "Invalid token or keyword or separator\n";
    cerr << "<identifier>,'if', 'ret', 'put', 'get','while' "
         << "keyword or '{'is expected at the beginning of a statement.\n";

    exit(1);
  }
}

// R21. <Compound> ::= { <Statement List> }
void Parser::Compound(ifstream &infile, ofstream &outfile) {
  if (lexeme == "{") {

    lexer(infile);

    StatementList(infile, outfile);
    if (lexeme == "}") {
      lexer(infile);

    } else {
      printError(outfile);
      outfile << "Compound syntax error\n";
      outfile << "Invalid separator, '}' is expected"
              << " at the end of statement list.\n";
      cerr << "Compound syntax error\n";
      cerr << "Invalid separator, '}' is expected"
           << " at the end of statement list.\n";

      exit(1);
    }
  } else {
    printError(outfile);
    outfile << "Compound syntax error\n";
    outfile << "Invalid separator, '{' is expected"
            << " at the beginning of statement list.\n";
    cerr << "Compound syntax error\n";
    cerr << "Invalid separator, '{' is expected"
         << " at the beginning of statement list.\n";

    exit(1);
  }
}

// R22. <Assign> ::= <Identifier> = <Expression> ;
void Parser::Assign(ifstream &infile, ofstream &outfile) {
  if (token == "identifier") {

    if (!CheckSymbol(lexeme)) {
      outfile << "no identifier! lexeme: " << lexeme << endl;
      cout << "no identifier! lexeme: " << lexeme << endl;
    }
    // save identifier lexeme
    string save = lexeme;
    temp = lexeme;
    lexer(infile);

    if (lexeme == "=") {
      lexer(infile);
      Expression(infile, outfile);
      int addr = GetAddress(save);
      GenerateInstruction("POPM", addr);

      if (lexeme == ";") {
        lexer(infile);

      } else {
        printError(outfile);
        outfile << "Assign syntax error\n";
        outfile << "Invalid separator, ';' is expected"
                   " at the end of assign statement.\n";
        cerr << "Assign syntax error\n";
        cerr << "Invalid separator, ';' is expected"
                " at the end of assign statement.\n";

        exit(1);
      }
    }

    else {
      printError(outfile);
      outfile << "Assign syntax error\n";
      outfile << "Invalid operator, '=' is expected"
              << " after <identifier> and before <expression>.\n";
      cerr << "Assign syntax error\n";
      cerr << "Invalid operator, '=' is expected"
           << " after <identifier> and before <expression>.\n";

      exit(1);
    }

  } else {
    printError(outfile);
    outfile << "Assign syntax error\n";
    outfile << "Invalid token, <identifier> is expected"
               " at the beginning of assign statement.\n";
    cerr << "Assign syntax error\n";
    cerr << "Invalid token, <identifier> is expected"
            " at the beginning of assign statement.\n";

    exit(1);
  }
}

// R23. <If> ::= if ( <Condition> ) <Statement> <If’>
void Parser::If(ifstream &infile, ofstream &outfile) {
  if (lexeme == "if") {

    // int address = instructionIndex;
    lexer(infile);

    if (lexeme == "(") {
      lexer(infile);

      Condition(infile, outfile);
      if (lexeme == ")") {
        lexer(infile);
        Statement(infile, outfile);
        BackPatch(instructionIndex);
        IfPrime(infile, outfile);
      } else {
        printError(outfile);
        outfile << "If statement syntax error\n";
        outfile << "Invalid separator, ')' is expected"
                << " after condition.\n";
        cerr << "If statement syntax error\n";
        cerr << "Invalid separator, ')' is expected"
             << " after condition.\n";

        exit(1);
      }
    } else {
      printError(outfile);
      outfile << "If statement syntax error\n";
      outfile << "Invalid separator, '(' is expected"
              << " before condition.\n";
      cerr << "If statement syntax error\n";
      cerr << "Invalid separator, '(' is expected"
           << " before condition.\n";

      exit(1);
    }
  } else {
    printError(outfile);
    outfile << "If statement syntax error\n";
    outfile << "Invalid keyword, 'if' keyword is expected"
            << " at the beginning of If statement.\n";
    cerr << "If statement syntax error\n";
    cerr << "Invalid keyword, 'if' keyword is expected"
         << " at the beginning of If statement.\n";

    exit(1);
  }
}

// R24. <If’> ::= endif | else <Statement> endif
void Parser::IfPrime(ifstream &infile, ofstream &outfile) {
  if (lexeme == "endif") {

    lexer(infile);

  } else if (lexeme == "else") {

    // int address = instructionIndex;
    lexer(infile);

    Statement(infile, outfile);

    if (lexeme == "endif") {
      lexer(infile);

    } else {
      printError(outfile);
      outfile << "If statement syntax error\n";
      outfile << "Invalid keyword, 'endif' is expected"
              << " at the end of If statement.\n";
      cerr << "If statement syntax error\n";
      cerr << "Invalid keyword, 'endif' is expected"
           << " at the end of If statement.\n";

      exit(1);
    }
  } else {
    printError(outfile);
    outfile << "If statement syntax error\n";
    outfile << "Invalid keyword, 'endif' is expected at the end of If "
               "Statement\n"
            << "Or 'else' is expected if there is an If-else statement.\n";
    cerr << "If statement syntax error\n";
    cerr << "Invalid keyword, 'endif' is expected at the end of If "
            "Statement\n"
         << "Or 'else' is expected if there is an If-else statement.\n";

    exit(1);
  }
}

// R25. <Return> ::= ret <Return’>
void Parser::Return(ifstream &infile, ofstream &outfile) {
  if (lexeme == "ret") {

    lexer(infile);

    ReturnPrime(infile, outfile);

  } else {
    printError(outfile);
    outfile << "Return statement syntax error\n";
    outfile << "Invalid keyword, 'ret' is expected"
            << " at the beginning of Return statement.\n";
    cerr << "Return statement syntax error\n";
    cerr << "Invalid keyword, 'ret' is expected"
         << " at the beginning of Return statement.\n";

    exit(1);
  }
}

// R26. <Return’> ::= ; | ret <Expression> ;
void Parser::ReturnPrime(ifstream &infile, ofstream &outfile) {
  if (lexeme == ";") {

    lexer(infile);

  } else {
    Expression(infile, outfile);
    if (lexeme == ";") {
      lexer(infile);

    } else {
      printError(outfile);
      outfile << "Return statement syntax error\n";
      outfile << "Invalid separator, ';' is expected"
              << " at the end of Return statement.\n";
      cerr << "Return statement syntax error\n";
      cerr << "Invalid separator, ';' is expected"
           << " at the end of Return statement.\n";

      exit(1);
    }
  }
}

// R27. <Print> ::= put ( <Expression>);
void Parser::Print(ifstream &infile, ofstream &outfile) {
  if (lexeme == "put") {
    lexer(infile);

    if (lexeme == "(") {
      lexer(infile);

      Expression(infile, outfile);
      if (lexeme == ")") {

        // write stdout code
        GenerateInstruction("STDOUT", BLANK);
        lexer(infile);

        if (lexeme == ";") {
          lexer(infile);

        } else {
          printError(outfile);
          outfile << "Print statement syntax error\n";
          outfile << "Invalid separator, ';' is expected"
                  << " at the end of Print statement.\n";
          cerr << "Print statement syntax error\n";
          cerr << "Invalid separator, ';' is expected"
               << " at the end of Print statement.\n";

          exit(1);
        }
      } else {
        printError(outfile);
        outfile << "Print statement syntax error\n";
        outfile << "Invalid separator, ')' is expected"
                << " before ';' and after <Expression>.\n";
        cerr << "Print statement syntax error\n";
        cerr << "Invalid separator, ')' is expected"
             << " before ';' and after <Expression>.\n";

        exit(1);
      }
    } else {
      printError(outfile);
      outfile << "Print statement syntax error\n";
      outfile << "Invalid separator, '(' is expected"
              << " after 'put' keyword and before <Expression>.\n";
      cerr << "Print statement syntax error\n";
      cerr << "Invalid separator, '(' is expected"
           << " after 'put' keyword and before <Expression>.\n";

      exit(1);
    }
  } else {
    printError(outfile);
    outfile << "Print statement syntax error\n";
    outfile << "Invalid keyword, 'put' is expected"
            << " at the beginning of Print statement.\n";
    cerr << "Print statement syntax error\n";
    cerr << "Invalid keyword, 'put' is expected"
         << " at the beginning of Print statement.\n";

    exit(1);
  }
}

// R28. <Scan> ::= get ( <IDs> );
void Parser::Scan(ifstream &infile, ofstream &outfile) {
  isFromRead = true;
  if (lexeme == "get") {
    lexer(infile);

    if (lexeme == "(") {
      lexer(infile);
      string save = lexeme;
      //print(outfile);
      IDs(infile, outfile);
      if (lexeme == ")") {
        int addr = GetAddress(save);
        GenerateInstruction("STDIN", BLANK);
        GenerateInstruction("POPM", addr);
        lexer(infile);
        //print(outfile);
        if (lexeme == ";") {
          lexer(infile);

        } else {
          printError(outfile);
          outfile << "Scan statement syntax error\n";
          outfile << "Invalid separator, ';' is expected"
                  << " at the end of Scan statement.\n";
          cerr << "Scan statement syntax error\n";
          cerr << "Invalid separator, ';' is expected"
               << " at the end of Scan statement.\n";
          exit(1);
        }
      } else {
        printError(outfile);
        outfile << "Scan statement syntax error\n";
        outfile << "Invalid separator, ')' is expected"
                << " after <IDs> and before ';'.\n";
        cerr << "Scan statement syntax error\n";
        cerr << "Invalid separator, ')' is expected"
             << " after <IDs> and before ';'.\n";
        exit(1);
      }
    } else {
      printError(outfile);
      outfile << "Scan statement syntax error\n";
      outfile << "Invalid separator, '(' is expected"
              << " after 'get' keyword and before <IDs>.\n";
      cerr << "Scan statement syntax error\n";
      cerr << "Invalid separator, '(' is expected"
           << " after 'get' keyword and before <IDs>.\n";
      exit(1);
    }
  } else {
    printError(outfile);
    outfile << "Scan statement syntax error\n";
    outfile << "Invalid keyword, 'get' is expected"
            << " at the beginning of Scan statement.\n";
    cerr << "Scan statement syntax error\n";
    cerr << "Invalid keyword, 'get' is expected"
         << " at the beginning of Scan statement.\n";
    exit(1);
  }
}

// R29. <While> ::= while ( <Condition> ) <Statement>
void Parser::While(ifstream &infile, ofstream &outfile) {
  if (lexeme == "while") {
    int addr = instructionIndex;
    GenerateInstruction("LABEL", BLANK);
    lexer(infile);

    if (lexeme == "(") {
      lexer(infile);
      temp = lexeme;

      Condition(infile, outfile);
      if (lexeme == ")") {
        lexer(infile);
        Statement(infile, outfile);
        GenerateInstruction("JUMP", addr);
        BackPatch(instructionIndex);
      } else {
        printError(outfile);
        outfile << "While statement syntax error\n";
        outfile << "Invalid separator, ')' is expected"
                << " after condition and before any statements.\n";
        cerr << "While statement syntax error\n";
        cerr << "Invalid separator, ')' is expected"
             << " after condition and before any statements.\n";

        exit(1);
      }
    } else {
      printError(outfile);
      outfile << "While statement syntax error\n";
      outfile << "Invalid separator, '(' is expected"
              << " before condition and after 'while' keyword.\n";
      cerr << "While statement syntax error\n";
      cerr << "Invalid separator, '(' is expected"
           << " before condition and after 'while' keyword.\n";

      exit(1);
    }
  } else {
    printError(outfile);
    outfile << "While-loop syntax error\n";
    outfile << "Invalid keyword, 'while' is expected"
            << " at the beginning of While-loop.\n";
    cerr << "While-loop syntax error\n";
    cerr << "Invalid keyword, 'while' is expected"
         << " at the beginning of While-loop.\n";

    exit(1);
  }
}

// R30. <Condition> ::= <Expression> <Relop> <Expression>
void Parser::Condition(ifstream &infile, ofstream &outfile) {
  Expression(infile, outfile);
  string op = lexeme;
  Relop(infile, outfile);
  Expression(infile, outfile);
  if (op == "=") {
    GenerateInstruction("EQ", BLANK);
    jumpStack.push(instructionIndex);
    GenerateInstruction("JUMPZ", BLANK);
  } else if (op == "/=") {
    GenerateInstruction("NEQ", BLANK);
    jumpStack.push(instructionIndex);
    GenerateInstruction("JUMPZ", BLANK);
  } else if (op == ">") {
    GenerateInstruction("GRT", BLANK);
    jumpStack.push(instructionIndex);
    GenerateInstruction("JUMPZ", BLANK);
  } else if (op == "<") {
    GenerateInstruction("LES", BLANK);
    jumpStack.push(instructionIndex);
    GenerateInstruction("JUMPZ", BLANK);
  } else if (op == "=>") {
    GenerateInstruction("GEQ", BLANK);
    jumpStack.push(instructionIndex);
    GenerateInstruction("JUMPZ", BLANK);
  } else if (op == "<=") {
    GenerateInstruction("LEQ", BLANK);
    jumpStack.push(instructionIndex);
    GenerateInstruction("JUMPZ", BLANK);
  } else {
    printError(outfile);
    outfile << "Relop syntax error\n";
    outfile << "Invalid operator\n"
            << "'=', '/=', '>', '<', '=>' or '<=' is expected"
            << " between 2 <Expression>.\n";
    cerr << "Relop syntax error\n";
    cerr << "Invalid operator\n"
         << "'=', '/=', '>', '<', '=>' or '<=' is expected"
         << " between 2 <Expression>.\n";

    exit(1);
  }
}

// R31. <Relop> ::= == | != | > | < | <= | =>
void Parser::Relop(ifstream &infile, ofstream &outfile) {
  if (lexeme == "==") {
    lexer(infile);

  } else if (lexeme == "!=") {
    lexer(infile);

  } else if (lexeme == ">") {
    lexer(infile);

  } else if (lexeme == "<") {
    lexer(infile);

  } else if (lexeme == "=>") {
    lexer(infile);

  } else if (lexeme == "<=") {
    lexer(infile);

  } else {
    printError(outfile);
    outfile << "Relop syntax error\n";
    outfile << "Invalid operator\n"
            << "'==', '!=', '>', '<', '=>' or '<=' is expected"
            << " between 2 <Expression>.\n";
    cerr << "Relop syntax error\n";
    cerr << "Invalid operator\n"
         << "'==', '!=', '>', '<', '=>' or '<=' is expected"
         << " between 2 <Expression>.\n";

    exit(1);
  }
}

// R32. <Expression> ::= <Term> <Expression'>
void Parser::Expression(ifstream &infile, ofstream &outfile) {
  Term(infile, outfile);
  ExpressionPrime(infile, outfile);
}

// R33. <Expression'> ::= + <Term> <Expression'> | - <Term> <Expression'> |
// <Empty>
void Parser::ExpressionPrime(ifstream &infile, ofstream &outfile) {
  if (lexeme == "+") {
    lexer(infile);
    CheckTypeMatch(prevLexeme, lexeme, infile, outfile);

    Term(infile, outfile);
    GenerateInstruction("ADD", BLANK);
    ExpressionPrime(infile, outfile);
  } else if (lexeme == "-") {
    lexer(infile);
    CheckTypeMatch(prevLexeme, lexeme, infile, outfile);

    Term(infile, outfile);
    GenerateInstruction("SUB", BLANK);
    ExpressionPrime(infile, outfile);
  } else {
    Empty(infile, outfile);
  }
}

// R34. <Term> ::= <Factor> <Term'>
void Parser::Term(ifstream &infile, ofstream &outfile) {
  Factor(infile, outfile);
  TermPrime(infile, outfile);
}

// R35. <Term'> ::= * <Factor> <Term'> | / <Factor> <Term'> | <Empty>
void Parser::TermPrime(ifstream &infile, ofstream &outfile) {
  if (lexeme == "*") {
    lexer(infile);
    CheckTypeMatch(prevLexeme, lexeme, infile, outfile);

    Factor(infile, outfile);
    GenerateInstruction("MUL", BLANK);
    TermPrime(infile, outfile);
  } else if (lexeme == "/") {
    lexer(infile);
    CheckTypeMatch(prevLexeme, lexeme, infile, outfile);

    Factor(infile, outfile);
    GenerateInstruction("DIV", BLANK);
    TermPrime(infile, outfile);
  } else {
    Empty(infile, outfile);
  }
}

// R36. <Factor> ::= - <Primary> | <Primary>
void Parser::Factor(ifstream &infile, ofstream &outfile) {
  if (lexeme == "-") {
    prevLexeme = lexeme;
    lexer(infile);

    Primary(infile, outfile);
  } else {
    Primary(infile, outfile);
  }
}

// R37. <Primary> ::= <Identifier> <Primary’>| <Integer> | (<Expression>) |
// <Real> | true | false
void Parser::Primary(ifstream &infile, ofstream &outfile) {
  if (token == "identifier") {
    if (!CheckSymbol(lexeme)) {
      outfile << "Identifier " << lexeme << " is not declared\n";
      cerr << "Idetnifier " << lexeme << " is not declared\n";

      exit(1);
    }
    if (!(GetType(temp) == GetType(lexeme)) && !(temp == "")) {
      outfile << "The type of " << temp << "and" << lexeme
              << "is not the same\n";
      cerr << "The type of " << temp << "and" << lexeme << "is not the same\n";

      exit(1);
    }
    int addr = GetAddress(lexeme);

    GenerateInstruction("PUSHM", addr);
    lexer(infile);

    PrimaryPrime(infile, outfile);

  } else if (token == "integer") {
    if (!(GetType(temp) == "integer") && !(temp == "")) {
      outfile << "The type of " << temp << "and" << lexeme
              << "is not the same\n";
      cerr << "The type of " << temp << "and" << lexeme << "is not the same\n";

      exit(1);
    }
    int a = 0;
    if (prevLexeme == "-") {
      prevLexeme += lexeme;
      a = stoi(prevLexeme);
    } else
      a = stoi(lexeme);
    GenerateInstruction("PUSHI", a);
    lexer(infile);

  } else if (lexeme == "(") {
    lexer(infile);

    Expression(infile, outfile);

    if (lexeme == ")") {
      lexer(infile);

    } else {
      printError(outfile);
      outfile << "Invalid separator, ')' is expected.\n";
      cerr << "Invalid separator, ')' is expected.\n";

      exit(1);
    }
  } else if (lexeme == "true") {
    if (!(GetType(temp) == "boolean") && !(temp == "") || prevLexeme == "-") {
      if (prevLexeme == "-") {
        outfile << "cannot assign" << temp << "to" << prevLexeme + lexeme
                << endl;
        cerr << "cannot assign" << temp << "to" << prevLexeme + lexeme << endl;
      } else {
        outfile << "The type of " << temp << "and" << lexeme
                << "is not the same\n";
        cerr << "The type of " << temp << "and" << lexeme
             << "is not the same\n";
        exit(1);
      }
      GenerateInstruction("PUSHI", 1);
      lexer(infile);

    } else if (lexeme == "false") {
      if (!(GetType(temp) == "boolean") && !(temp == "") || prevLexeme == "-") {
        if (prevLexeme == "-") {
          outfile << "Cannot assign " << temp << " to " << prevLexeme + lexeme
                  << endl;
          cerr << "Cannot assign " << temp << " to " << prevLexeme + lexeme
               << endl;
        } else {
          outfile << "The type of " << temp << " and " << lexeme
                  << " must match" << endl;
          cerr << "The type of " << temp << " and " << lexeme << " must match"
               << endl;
        }

        exit(1);
      }
      GenerateInstruction("PUSHI", 0);
      lexer(infile);

    } else {
      printError(outfile);
      outfile << "Primary syntax error\n";
      outfile << "Invalid token, separator, or boolean value\n";
      outfile << "<identifier>, <integer>, <real>, "
              << "'(', 'true' or 'false' is expected"
              << " after '+', '-', '*' or '/'.\n";
      cerr << "Primary syntax error\n";
      cerr << "Invalid token, separator, or boolean value\n";
      cerr << "<identifier>, <integer>, <real>, "
           << "'(', 'true' or 'false' is expected"
           << " after '+', '-', '*' or '/'.\n";

      exit(1);
    }
  }
}
// R38. <Primary’> ::=  ( <IDs> ) | <Empty>
void Parser::PrimaryPrime(ifstream &infile, ofstream &outfile) {
  if (lexeme == "(") {
    lexer(infile);

    IDs(infile, outfile);
    if (lexeme == ")") {
      lexer(infile);

    } else {
      printError(outfile);
      outfile << "Primary syntax error\n";
      outfile << "Invalid separator, ')' is expected"
              << " after <IDs>.\n";
      cerr << "Primary syntax error\n";
      cerr << "Invalid separator, ')' is expected"
           << " after <IDs>.\n";
      exit(1);
    }
  } else {
    Empty(infile, outfile);
  }
}

// R39. <Empty> ::= epsilon
void Parser::Empty(ifstream &infile, ofstream &outfile) {
  ; // Do nothing in this function
}

void Parser::printError(ofstream &outfile) {
  outfile << "Error at line " << lineNum << endl;
  cerr << "Error at line " << lineNum << endl;
  // Reset the line number if there is an syntax error.
  lineNum = 1;
}

// Destructor
Parser::~Parser() {
  memoryAddr = 7000;
  countSymbol = 0;
  isFromRead = false;
  isFromDeclaration = false;
  prevLexeme = "";
  temp = "";
}
