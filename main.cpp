#include <iostream>
#include <fstream>
#include <string>

// Enum class TokenType is to enumerate between different token types
enum class TokenType {
    KEYWORD, IDENTIFIER, OPERATOR, REAL, INTEGER, SEPARATOR, ERROR, END_OF_FILE
};

// Function to convert TokenType enum values to string
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD:
            return "KEYWORD";
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        case TokenType::OPERATOR:
            return "OPERATOR";
        case TokenType::REAL:
            return "REAL";
        case TokenType::INTEGER:
            return "INTEGER";
        case TokenType::SEPARATOR:
            return "SEPARATOR";
        case TokenType::ERROR:
            return "ERROR";
        case TokenType::END_OF_FILE:
            return "END_OF_FILE";
    }
    return "UNKNOWN";
}

// Structure to represent a token
struct Token {
    TokenType type;
    std::string lexeme;
};

// Lexer class
class Lexer {
private:
    std::ifstream file; // Input file stream
    char current_char; // Current character being processed
  int currentState = 1; // Initialize currentState with the value 1
  int temp = currentState; // Copy the value of currentState to temp

    // Helper function to check if a character is whitespace
    bool isWhitespace(char c) {
        return (c == ' ' || c == '\t' || c == '\n');
    }

    // Helper function to check if a character is an operator
    bool isOperator(char c) {
        return (c == '+' || c == '-' || c == '*' || c == '/' ||
                c == '<' || c == '>' || c == '=' || c == '!');
    }

    // Helper function to check if a character is a separator
    bool isSeparator(char c) {
        return (c == '(' || c == ')' || c == ';' || c == ',' ||
                c == '[' || c == ']' || c == '{' || c == '}');
    }

	void switchStates(int switchTo){
		//set new state
		temp = switchTo;
	}

	int IdStateCheck(){
		//return current state
		return temp;
	}
  Token ValidState(std::string lexeme){
    if (IdStateCheck() == 4){
			return { TokenType::IDENTIFIER, lexeme };
		}
		else {
      std::cout << IdStateCheck() << " " << lexeme << std::endl;
			return {TokenType::ERROR, lexeme};
		}
  }

    // Function to scan and recognize an identifier
	Token scanIdentifier() {
    //parse identifier and set state
		std::string lexeme;
		while (std::isalpha(current_char) || std::isdigit(current_char)){
			if (std::isalpha(current_char)){
				switchStates(2);
				lexeme += current_char;
				file.get(current_char);
			}
			else if (std::isdigit(current_char)){
				if (IdStateCheck() != 1){
					switchStates(3);
					lexeme += current_char;
					file.get(current_char);
				}
				else{
					switchStates(5);
					return ValidState(lexeme);
				}
     }
		}
    if (current_char == ' ' || current_char == '\n' || isSeparator(current_char)){
      if (IdStateCheck() == 2) {
        switchStates(4);
      }
      else {
        switchStates(5);
      }
    }
		// Check if the lexeme matches any of the keywords
		if (lexeme == "while") {
			return { TokenType::KEYWORD, lexeme }; // Recognized as a keyword
		}
    return ValidState(lexeme);
	}


    // Function to scan and recognize a real number or integer
  Token scanReal() {
        std::string lexeme;
        while (std::isdigit(current_char)) {
            lexeme += current_char;
            file.get(current_char);
        }
        if (current_char == '.') {
            lexeme += current_char;
            file.get(current_char);
            while (std::isdigit(current_char)) {
                lexeme += current_char;
                file.get(current_char);
            }
            return { TokenType::REAL, lexeme };
        }
        return { TokenType::INTEGER, lexeme };
    }

    // Function to scan and recognize an operator
    Token scanOperator() {
        std::string lexeme;
        if (current_char == '<' || current_char == '>' || current_char == '=' || current_char == '!') {
            lexeme += current_char;
            file.get(current_char);
            if (current_char == '=') {
                lexeme += current_char;
                file.get(current_char);
            }
        }
        else {
            lexeme += current_char;
            file.get(current_char);
        }
        return { TokenType::OPERATOR, lexeme };
    }

    // Function to scan and recognize a separator
    Token scanSeparator() {
        std::string lexeme(1, current_char);
        file.get(current_char);
        return { TokenType::SEPARATOR, lexeme };
    }

public:
    // Constructor takes the input filename and opens the file
    Lexer(const std::string& filename) {
        file.open(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open the input file." << std::endl;
            exit(1);
        }
        file.get(current_char); // Initialize current_char with the first character
    }

    // Function to get the next token from the input file
    Token getNextToken() {
        while (!file.eof()) {
            if (isWhitespace(current_char)) {
                while (isWhitespace(current_char))
                    file.get(current_char); // Skip whitespace
            }
            else if (std::isalpha(current_char) || current_char == '_') {
              return scanIdentifier(); // Scan and recognize an identifier
              
            }
            else if (std::isdigit(current_char)) {
                return scanReal(); // Scan and recognize a real number or integer
            }
            else if (isOperator(current_char)) {
                return scanOperator(); // Scan and recognize an operator
            }
            else if (isSeparator(current_char)) {
                return scanSeparator(); // Scan and recognize a separator
            }
            else {
                // Handle other cases as needed (e.g., report an error for unexpected characters)
                std::cerr << "Error: Unexpected character '" << current_char << "'" << std::endl;
                exit(1);
            }
        }
        return { TokenType::END_OF_FILE, "" }; // End of file reached
    }
};

int main() {
    // Create a lexer with the input file "source_code.txt"
    Lexer lexer("test.txt"); // Use double quotes for the filename

    // Declare a Token variable to store the current token
    Token token;
    
    // Print the header for the output
    std::cout << "Output:" << std::endl;
    std::cout << "token lexeme" << std::endl;

    // Loop to tokenize the input file
    while (true) {
        // Get the next token from the lexer
        token = lexer.getNextToken();
        

        // Check if the end of the file is reached
        if (token.type == TokenType::END_OF_FILE)
            break; // End the loop

        // Print the token type and lexeme in the specified format
        std::cout << tokenTypeToString(token.type) << " " << token.lexeme << std::endl;
    }

    // Return 0 to indicate successful execution
    return 0;
}
