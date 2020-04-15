/*
 * Lexer.h
 *
 *  Created on: Jul 1, 2018
 *      Author: Dakota
 * Header file for Lexer.cpp, includes def of the CharClassifier class for classifying char types
 */

#ifndef LEXER_H_
#define LEXER_H_

#include <utility> //pair

#include "Operators.h"
#include "utils/Twine_Utils.h"
#include "Token.h"

#include "Flags.h"

using namespace std;


vector<string> importedFiles = vector<string>();//keep track of files so we dont include the same file twice
vector<pair<string, string>> definedAliases = vector<pair<string, string>>();//keep track of pairs

bool lex(const string* data, vector<TokenData>* tokens);
bool isNum(char data);
bool isOp(char data);
bool scrub(vector<TokenData>* tokens);
void addLitteral(TokenData i, vector<TokenData>* tokens);
bool isReserved(const string& text);
bool isValidNum(const string& text);
bool Lexer(string* data, vector<TokenData>* tokens);
void checkControleLine(const string& line, int lineNumber, vector<TokenData>* tokens);
//Import functions//
void parseImport(const string& name, vector<TokenData>* tokens);
bool download(const string& URL);


class CharClassifier{
public:
  void init();
  enum Type{//not sure how many of these are actually used
	    WHITESPACE,//0,               not generally used
	    LINE_BREAK,//1,               ; not generally used
	    LETTER,//2,                   abcs...ABCs...
	    DIGIT,//3,                    123...
	    OPERATOR,//4,                 +-=....
	    STRING_QUOTE,//5,             "
	    SINGLE_LINE_COMMENT,//6,      not generally used
	    MULTI_LINE_COMMENT_START,//7, not generally used
	    MULTI_LINE_COMMENT_END,//8,   not generally used
	    OTHER,//9,                    Non-ASCII chars as unicode is not currently supported and normally means there is data corruption/misread
	    INVALID//10                   not generally used
  };
  
  string TypeNames[11];//used for debug, printing type of individual chars
  static inline TokenData::Type getTokenType(CharClassifier::Type type, TokenData::Type previousType);
  inline CharClassifier::Type get(char C);
  unordered_map<char, CharClassifier::Type> tk;
  
}charClassy;//used in Lex




#endif /* LEXER_H_ */
