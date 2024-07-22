#include "Lex.h"
#include "Par.h"
#include <ctype.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

int main() {
  ifstream inFile;
  ofstream outFile("output.txt");
  string filename;
  string line;

  cout << "Enter test case name (ex: test.txt): ";
  cin >> filename;

  inFile.open(filename.c_str());

  // Make sure file is opened
  if (!inFile.is_open()) {
    cerr << "Failed to open the file.\n";
    cerr << "Please enter correct file.\n";
    exit(1);
  } 
  
  Parser check;

  inFile.clear();
  inFile.seekg(0);

  // Run parser
  check.RAT23F(inFile, outFile);
  
  // Print symbol table and sssembly code
  check.PrintSymbol(inFile, outFile);
  check.PrintInstruction(inFile, outFile);
  inFile.close();

  cout << endl;
  outFile << endl;

  outFile.close();
  cout << "see output.txt for results" << endl;

  return 0;
}