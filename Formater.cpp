//Feb 27, 2019
//D.A

//auto formats .tw file to have correct indentation and newlines
//still missing newline before }
//#include "Flags.h"
#include "utils/Twine_Utils.h"

void ensure(string sym);//checks for a sym, adding it if its missing
void ensureEndBlock();
void ensureNewLine(bool endOfBlock = true);
void ensureIndentation(int indent);//ensures there are enough spaces or tabs
string nextString();

int i = 0;
vector<TokenData> * tokens;

//simple non-parser formatter
void Format(vector<TokenData> * t){
  tokens = t;
  
  out = "";
  int expectIndent = 0;
  bool expectNewline = false;
  for(; i < tokens->size()-1; i++){
    if(tokens->at(i).tokenType == TokenData::FILE_END) break;
    if(tokens->at(i).tokenType == TokenData::FILE_CHANGE) continue;
    out += tokens->at(i).tokenText;
    if(tokens->at(i).tokenText == "{"){
      if(getFlag("FIX_BLOCK_NEWLINES"))
	expectNewline = true;
      expectIndent++;
    }else if(tokens->at(i).tokenText == "}"){
      if(!expectIndent);//missmatched {}
      expectIndent--;
      if(nextString() != "else" && nextString() != "elif"){
	expectNewline = true;
      }else if(getFlag("FIX_DOULBE_CLOSING") && tokens->at(i+1).tokenText == "}")//not really used
	 expectNewline = true;
    }
    if(expectNewline && tokens->at(i+1).tokenText != "\n")
      out+="\n";
    expectNewline = false;
    if(getFlag("FIX_INDENTATION") && out.back() == '\n'){
      int j = 0;
      if(tokens->at(i+1).tokenText == "}"){
	j++;
      }
      ensureIndentation(expectIndent - j);
    }
  }//end main for loop

  if(expectIndent);//missmatched {}
  
}
string nextString(){
  return tokens->at(i+1).tokenText;
}

void ensure(string sym){
  if(tokens->at(i).tokenText == sym)
    i++;
  out+=sym;
}

void ensureEndBlock(){
  if(!getFlag("FIX_BLOCK_NEWLINES"))
    return;
  if(nextString() == "else" || nextString() == "elif"){
    //fine
  }else{
    ensure("\n");
  }
}

void ensureNewLine(bool endOfBlock){
  if(endOfBlock){
    /*if(getFlag("FIX_BLOCK_NEWLINES")){
      ensure("\n");
      }*/
    ensureEndBlock();
  }else{
    if(getFlag("ONE_LINE_PER_LINE")){
      ensure("\n");
    }
  }
}

void ensureIndentation(int indent){
  for(int j = 0; j<indent; j++){
    if(getFlag("USE_SPACES")){
      ensure(" ");ensure(" ");
    }else{
      ensure("\t");
    }
  }
}
