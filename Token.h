
/*
 * Token.h
 *
 *  Created on: Jul 2, 2018
 *      Author: Dakota
 * This file hold a class TokenData
 * TokenData is used to stor data aout tokens obtained from lexing a file in Lexer.cpp
 * This data is the type of token, position of it int he original file, and the actual text of the token.
 */

#ifndef TOKEN_H_
#define TOKEN_H_

#include <string.h>
#include <iostream>
using namespace std;

class TokenData{
public:  
  enum Type{
	    WHITESPACE,//used for lint
	    LINE_END,//not need/not used
	    IDENT,
	    LITERAL,
	    STRING_LITERAL,
	    OPERATOR,
	    LINE_COMMENT,
	    BLOCK_COMMENT,
	    NUM,
	    RESERVED,
	    END_OF_FILE,
	    FORCE_ADDED,
	    BAD_TYPE,
	    FILE_CHANGE, //change what file were parsing
	    DELIMITER,//; and ,
	    IMPORT,
  };
  string tokenText;
  unsigned line, charPos;
  Type tokenType;
  
  TokenData(){tokenType = BAD_TYPE;}//This shouldnt be called
  TokenData(string textIn,int lineIn, int charPosIn, Type tokenTypeIn){//main constructor
    tokenText=textIn;
    line=lineIn;
    charPos=charPosIn;
    tokenType=tokenTypeIn;
  }
  static string getTokenTypeName(Type t){//for debug
    switch(t){
    case(NUM):
      return "NUM";
    case(RESERVED):
      return "RESERVED";
    case(WHITESPACE):
      return "WHITESPACE";
    case(LINE_END):
      return "LINE_END";
    case(IDENT):
      return "IDENTIFIER";
    case(LITERAL):
      return "LITERAL";
    case(STRING_LITERAL):
      return "STRING_LITERAL";
    case(OPERATOR):
      return "OPERATOR";
    case(LINE_COMMENT):
      return "LINE_COMMENT";
    case(BLOCK_COMMENT):
      return "BLOCK_COMMENT";
    case(END_OF_FILE):
      return "EndOfFile";
    case(FILE_CHANGE):
      return "FILE_CHANGE";
    case(DELIMITER):
      return "DELIMITER";
    case(IMPORT):
      return "IMPORT";
    default:
      return "BAD_TYPE";
    }
  }
  
}TokenClassy;//is used in Lexer.cpp lex()


#endif //Token_H_
