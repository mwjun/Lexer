#include "Lex.h"

int Lexer::lineNum = 1;

// constructor
Lexer::Lexer() { input = 'c'; }

// Function to check if separator
bool Lexer::isSeparator(const char input) const {
  int separator[7] = {'(', ')', '{', '}', '#', ',', ';'};
  for (int i = 0; i < 7; i++) {
    if (separator[i] == input) {
      return 1;
    }
  }
  return 0;
}
// Function to check if Operator
bool Lexer::isOperator(const char input) const {
  int operators[8] = {'+', '-', '/', '*', '<', '>', '=', '!'};
  for (int i = 0; i < 8; i++) {
    if (operators[i] == input) {
      return 1;
    }
  }
  return 0;
}

// Function to check if keyword
bool Lexer::isKeyword(string identifier) const {
  string keywords[12] = {"while", "if", "endif", "else",  "ret",  "get",
                         "put",   "integer", "true",  "false", "bool", "real"};
  for (int i = 0; i < 12; i++) {
    if (keywords[i] == identifier) {
      return 1;
    }
  }
  return 0;
}

int Lexer::Classify(string s) {
  int len = s.length();

  // detect is operator or not
  for (int i = 0; i < len; i++) {
    if (isOperator(s[i]))
      return 1;
  }

  // detect is seperator or not
  for (int i = 0; i < len; i++) {
    if (isSeparator(s[i]))
      return 2;
  }

  // detect is identifier or not
  char classify_ch = s[0];
  if (isalpha(classify_ch)) {
    for (int i = 0; i < len; i++) {
      // if char is either num or letter, keep checking the string sequence
      // else return 6 which is invalid input
      if (isalpha(s[i]) || isdigit(s[i]))
        ;
      else
        return 6; // invalid input
    }
    return 3;
  } else if (isdigit(classify_ch)) {
    // first, check for valid input for real
    // only accept the string with number or dot(.) sign
    for (int i = 0; i < len; i++) {
      if (s[i] == '.' || isdigit(s[i]))
        ;
      else
        return 6;
    }

    // second, check if there is a dot, then string could be a real number
    for (int i = 0; i < len; i++) {
      if (s[i] == '.')
        return 4;
    }

    // detect is integer
    return 5;
  } else
    return 6; // invalid input

  // return random number here
  return 7;
}

// Function returns the column number of the character in the table
int Lexer::char_to_col(const char input) const {
  if (isalpha(input))
    return 1;
  else if (isdigit(input))
    return 2;
  else if (isOperator(input) || isSeparator(input))
    return 3;
  else
    return 4;
}

// char to column but for real
int Lexer::char_to_col_real(const char input) const {
  if (isdigit(input))
    return 1;
  else if (input == '.')
    return 2;
  else
    return 3;
}

// Finite State Machine for integer
int Lexer::int_DFSM(const string str) {
  // starting state
  int state = 1;

  // create table N for the transitions or DFSM table for integer
  /*	0	d
    1	2
    2	2
  */
  int a[3][2] = {{0, 'd'}, {1, 2}, {2, 2}};

  // accepting states
  int f[1] = {2};

  // update the currentState to new transition
  int size = str.size();
  for (int i = 0; i < size; i++) {
    // convert the char to column number in table
    int col = char_to_col(str[i]);

    // update the current state
    state = a[state][col];
  }
  if (state == f[0])
    return 1;
  else
    return 0;
}

// Finite State Machine for real
int Lexer::real_DFSM(string str) {
  // starting state
  int state = 1;

  // DFSM table for real
  /*	0	d	.
      1	2	0
      2	2	3
      3	4	0
      4	4	0
  */
  int a[5][3] = {{0, 'd', '.'}, {1, 2, 0}, {2, 2, 3}, {3, 4, 0}, {4, 4, 0}};

  int f[1] = {4};

  // convert character to its column number in the table
  int size = str.size();
  for (int i = 0; i < size; i++) {
    int col = char_to_col_real(str[i]);
    state = a[state][col];

    // if state = 0, then it is a failing state. Reject immediately
    if (state == 0)
      return 0;
  }
  if (state == f[0])
    return 1;
  else
    return 0;
}

// Finite State Machine for identifier
int Lexer::identifier_DFSM(string str) {
  // starting state
  int state = 1;

  // transition table
  // failing state = 0
  // accepting state = 4
  /*	l	d	i	\n
    1	2	0	0	0
    2	2	3	0	4
    3	2	3	0	0
    4	4 4	0	4
  */
  int a[5][5] = {{0, 'l', 'd', 'i', '\n'},
                 {1, 2, 0, 0, 0},
                 {2, 2, 3, 0, 4},
                 {3, 2, 3, 0, 0},
                 {4, 4, 4, 0, 4}};

  int f[2] = {4, 2};

  // convert character to its column number in the table
  int size = str.size();
  for (int i = 0; i < size; i++) {
    int col = char_to_col(str[i]);
    state = a[state][col];
    if (state == 0)
      return 0;
  }

  for (int i = 0; i < 4; i++) {
    if (state == f[i])
      return 1;
  }
  return 0;
}

void Lexer::lexer(ifstream &file) {
  string str; // string stores the lexeme
  int state_status = 0;
  bool found = false;
  char ch = 'c', prevChar = 'c';

  // get the character and add it into a string until see space, separator, or
  // operator
  while (!found) {
    // Update line number for Error Handler in Par.h
    if (prevChar == '\n') {
      lineNum++;
    }
    ch = file.get();
    prevChar = ch;

    // check if input is a comment
    if (ch == '[') {
      ch = file.get();
      if (ch == '*') {
        ch = file.get();
        while (ch != '*') {
          ch = file.get();
          prevChar = ch;
        }
        ch = file.get();
        if (ch == ']' && prevChar == '*') {
          // comment successfully skipped
          ch = file.get();
        }
      }
    }

    // check if current character is a separator, operator, whitespace, or eof
    // if yes, put the flag to exit the loop
    if (this->isSeparator(ch) || this->isOperator(ch) || isspace(ch) ||
        ch == -1) {
      found = true;
    }

    /*if string is not empty and current character is either operator or
    separator decrease the current location in stream by one character else if
    current character is neither whitespace nor eof stores the char into
    string*/
    if (!str.empty() && (this->isOperator(ch) || this->isSeparator(ch)))
      file.unget();
    else if (!isspace(ch) && !(ch == -1))
      str += ch;

    // if the string is empty and current is not eof, get back to the loop
    // used to skip the whitespaces
    if (str.empty() && !(ch == -1))
      found = false;
  }

  // handle the file.txt with extra whitespaces at the end of the file
  if (str.empty() && ch == -1) {
    this->setLexeme("EOF");
    this->setToken("EOF");
    return;
  }

  int classify = Classify(str);

  // check token using FSM for identifier
  if (classify == 3) {

    // use FSM-identifier to check if token is accepted or not
    state_status = identifier_DFSM(str);

    this->setLexeme(str);
    if (state_status == 1) {
      if (isKeyword(str))
        this->setToken("keyword");
      else
        this->setToken("identifier");
    } else {
      this->setToken("invalid identifier");
    }
  }
  // check for operator
  else if (classify == 1) {
    str = ch;

    // return the next char without extracting it from input sequence
    ch = file.peek();

    /*check for valid operators: ==, !=, <=, =>
    if current char and next char is a valid operator
    add next char to string and move to the next location char
    to keep track of the checking*/
    if ((str[0] == '=' && ch == '=') || (str[0] == '!' && ch == '=') ||
        (str[0] == '<' && ch == '=') || (str[0] == '=' && ch == '>')) {
      str += ch;
      file.get();
    }

    // reject invalid operators if neccessary
    if (isOperator(str[0]) || str == "==" || str == "!=" || str == "<=" ||
        str == "=>") {
      this->setToken("operator");
      this->setLexeme(str);
    } else {
      this->setToken("invalid operator");
      this->setLexeme(str);
    }
  }
  // check for separator
  else if (classify == 2) {
    str = ch;
    ch = file.peek();
    if (isSeparator(str[0])) {
      this->setLexeme(str);
      this->setToken("separator");
    } else {
      this->setLexeme(str);
      this->setToken("invalid separator");
    }
  }
  // check token using FSM for real
  else if (classify == 4) {
    state_status = real_DFSM(str);
    this->setLexeme(str);
    if (state_status == 1) {
      this->setToken("real");
    } else {
      this->setToken("invalid real");
    }
  }
  // check token using FSM for int
  else if (classify == 5) {
    state_status = int_DFSM(str);
    this->setLexeme(str);

    if (state_status == 1) {
      this->setToken("integer");
    } else
      this->setToken("invalid integer");
  } else {
    this->setLexeme(str);
    this->setToken("invalid input");
  }
}

void Lexer::print() const {
  cout << "Token: " << left << setw(20) << this->token << "Lexeme: " << setw(20)
       << this->lexeme << endl;
}

void Lexer::setToken(const string newToken) { token = newToken; }

void Lexer::setLexeme(const string newLexeme) { lexeme = newLexeme; }

string Lexer::getToken() const { return token; }

string Lexer::getLexeme() const { return lexeme; }

// destructor
Lexer::~Lexer() {}