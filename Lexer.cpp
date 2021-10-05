
/*
 * TwineComp.cpp
 * Takes raw text(string) and tokenizes(vector<TokenData>) it
 *
 * TODO add support(change to) UTF-8
 * Created on: Jul 1, 2018
 *      Author: Dakota
 */

#include "Lexer.h"

//Gets classifier for single char
inline CharClassifier::Type CharClassifier::get(char C){
    return this->tk.find((unsigned char)C)->second;//breaks here when using unicode
}

/*This is the function called to lex. It calls the main tokeniser and the lexer if appropriate*/ 
bool Lexer(const string * data, vector<TokenData>* tokens){
  if(data->size() == 0) return false;//error if there is nothing
  
  if(!tokens->capacity())
    tokens->reserve((data->size()/6)+2);
  
  if(lex(data, tokens)){  
    return true;
  }
  
  return false;
}

/*
 *tokenization a string
 *
 *Parameters: string * data              : The data to be tokenized 
 *            vector<TokenData>* tokens  : Where to push the tokens to
 *
 *Returns bool : Wheter the tokenization was succsessfull 
 */
bool lex(const string* data, vector<TokenData>* tokens){//TODO add lexer parts(checking indents for {,}, new lines, etc..

  charClassy.init();//sets up unordered map for char calssification

  string tok = "";//string to hold the new chars
  unsigned line, charPos = 1;//position of token, start with line # being negitive two as all files start with to added blank lines
  char C, lastChar;
  if(interpMode){line=1;}else{line = -1;}

  CharClassifier::Type lastCharClass = CharClassifier::WHITESPACE;//initial starting conditions
  bool inComment = false, multiComment = false, StringLit = false, controleLine = false;
  CharClassifier::Type charType;

  report("Lexing file",0);

  for(unsigned i=0; i<data->length(); ++i){ //loop through ever char in the input file
    C = data->at(i);//get the next char
    charType = charClassy.get(C);//get char type (digit, letter, operator, etc..)
    charPos++;
    if(C == '\n'){//start of complex IF's
      if(inComment && !multiComment){//end of a single line comment
        if(SAVE_LINE_COMMENTS) tokens->emplace_back(tok, line, charPos-tok.length(), TokenData::LINE_COMMENT);
        tok = "";
        inComment = false;
      }else if(StringLit || inComment){
	tok += C;
      }else if(controleLine){
	//tokens->push_back(TokenData(tok, line, charPos-tok.length(), TokenData::FILE_CHANGE));
	//tokens->push_back(checkControleLine(tok, line));
	int tmp = line;
	checkControleLine(tok, line, tokens);
	controleLine = false;
	tok = "";
	line = tmp;
      }else{
	if(tok != ""){
	  //tokens->push_back(TokenData(tok, line, charPos-tok.length(), TokenData::LITERAL));//push any unpushed tokens
	  addLitteral(TokenData(tok, line, charPos-tok.length(), TokenData::LITERAL), tokens);
	  tok = "";
	}
      }
      if(SAVE_WHITESPACE && !inComment && !StringLit) tokens->emplace_back("\n", line, charPos, TokenData::LINE_END);
      charPos = 0;//TODO does this work with strings with new line?
      line++;
    }else if(C == '/' && lastChar == '/' && !StringLit && !controleLine){//start of single line comment
      if(tokens->size() != 0 && tokens->back().tokenText == "/"){//TODO
	tokens->erase(tokens->end());
      }
      tok += '/';
      inComment = true;
      tok += C;
    }else if(C == '*' && lastChar == '/' && !StringLit){//start of multi line comment
      tok += '/';
      if(!inComment){//only make new comment if not already in one
	tokens->erase(tokens->end());// prevents "// /*" from being start of multiline comment
	inComment = true;
	multiComment = true;
      }
      tok += C;
    }else if(C == '/' && lastChar == '*' && multiComment){//end of a multi line
      inComment = false;
      multiComment = false;
      tok += C;
      if(SAVE_BLOCK_COMMENTS) tokens->emplace_back(tok, line, charPos-tok.length(), TokenData::BLOCK_COMMENT);
      tok = "";
    }else if(charType == CharClassifier::STRING_QUOTE && !inComment){
      if(StringLit && lastChar != '\\'){//end string literal
	tok += C;
	StringLit = false;
	tokens->emplace_back(tok, line, charPos-tok.length(), TokenData::STRING_LITERAL);
	tok = "";
      }else if(!StringLit && lastChar != '\\'){//start of string literal
	if(tok != ""){
	  addLitteral(TokenData(tok, line, charPos-tok.length(), TokenData::LITERAL), tokens);
	  tok = ""; 
	}
	tok += C;
	StringLit = true;
      }else{
	tok += C;
      }
    }else if(C == '#' && !StringLit && !inComment){
      controleLine = true;
    }else{
      if(inComment || StringLit || controleLine){//nothing more to do if in comment/literal
	tok += C;
      }else if(charType == CharClassifier::WHITESPACE){
	//do nothing, this isn't Python
	if(tok != ""){
	  addLitteral(TokenData(tok, line, charPos-tok.length(), TokenData::LITERAL), tokens);
	  tok = "";
	}
	if(SAVE_WHITESPACE){
	  tok += C;
	  tokens->emplace_back(tok, line, charPos-tok.length(), TokenData::WHITESPACE);
	  tok = "";
	}
      }else if(charType == CharClassifier::LINE_BREAK || C == ','){//line break ';'
	if(tok != ""){
	  addLitteral(TokenData(tok, line, charPos-tok.length(), TokenData::LITERAL), tokens);
          tok = "";
	}
	tok += C;
        tokens->emplace_back(tok, line, charPos-tok.length(), TokenData::DELIMITER);//explicit end lines 
        tok = "";
      }else if(charType == CharClassifier::DIGIT && lastChar == '.'){//these two blocks helps with the '.' in doubles
	if(tok == ""){
	  tokens->erase(tokens->end());
	  tok += '.';
	} 
        tok += C;
      }else if(C == '.' && lastCharClass == CharClassifier::DIGIT){
        tok += C;
      }else if(charType == CharClassifier::OPERATOR){//any operators: +,=,-,(,[ etc..
	if(tok != ""){
	  addLitteral(TokenData(tok, line, charPos-tok.length(), TokenData::LITERAL), tokens);
	  tok = "";
	}else{
	  if(lastCharClass == CharClassifier::OPERATOR){//checks for valid 2 char ops
	    string test = "";
	    test += lastChar;
	    test += C;
	    for(int x = 0; x<sizeof(OPS2)/sizeof(OPS2[0]);x++){//TODO use master list of OPs? would be slower but would keep everything in one place
	      if(test == OPS2[x]){
		tok += lastChar;
		tokens->erase(tokens->end());//erase last token since it was single OP, it gets replaced by 2 char OP
		break;
	      }
	    }//end for(x)
	  }//if(lastCharClass == CharClassifier::OPERATOR)
	}//else
	tok += C;
	tokens->emplace_back(tok, line, charPos-tok.length(), TokenData::OPERATOR);//operators
	tok = "";
      }else if(lastCharClass == charType && lastCharClass != CharClassifier::WHITESPACE){//matching types
	tok += C;
      }else{
	if(tok != "" && tok != "\n"){
	  if(charType == CharClassifier::INVALID)//non-valid ASCII chars found, such as bell, backspace, etc
	    report("WARNING: Found invalid char while parsing at line: "+to_string((long long) line)+" char position: "+to_string((long long) charPos),2);
	  addLitteral(TokenData(tok, line, charPos-tok.length(), TokenData::LITERAL), tokens);
	  tok = "";
	}
	tok += C;
      }
    }
    lastCharClass = charType;
    lastChar = C;
  }//end FOR loop

  //post check of input stream, because, you know, function wasn't long enough already
  if(multiComment){//checks if still in multi line comment at end of file
    report("Warning, reached EOF while in comment",-1);
    if(SAVE_BLOCK_COMMENTS){
      tok += "*/";
      tokens->emplace_back(tok, line, charPos-tok.length(), TokenData::BLOCK_COMMENT);
    }
    tok = "";
  }
  if(tok != ""){//check if tok stream is not empty
    //tokens->push_back(TokenData(tok, line, charPos-tok.length(), TokenData::LITERAL));
    addLitteral(TokenData(tok, line, charPos-tok.length(), TokenData::LITERAL), tokens);
  }
  if(StringLit){//checks if still in string
    report("ERROR: miss matched '\"', still in string when reached end of file while parsing at line: "+to_string((long long) line)+" char position: "+to_string((long long) charPos),4);
    report(to_string(tok.size()),-2);
    //report("Token was: "+tok, -2);
    if(FORCE){
      report("Miss matched '\"', still in string when reached end of file while parsing at line, closing quote for you",-1);
      tok += '\"';
      tokens->emplace_back(tok, line, charPos-tok.length(), TokenData::STRING_LITERAL);
    }else{
      return false;
    }
  }
  tokens->emplace_back("EOF", line, charPos, TokenData::END_OF_FILE);
  tokens->emplace_back("EOF", line, charPos, TokenData::END_OF_FILE);
  //TODO free memory
  report("Success Lexing file",0);
  return true;
}

//scrubs through the list of tokens and sets them accordingly
//this is nop longer used
bool scrub(vector<TokenData>* tokens){
  report("Scrubbing",0);
  for(vector<TokenData>::iterator i = tokens->begin(); i != tokens->end(); i++){
    if(i->tokenType == TokenData::LITERAL){//checks for numbers
      if(isNum(i->tokenText.at(0)) || (i->tokenText.at(0) == '.' && isNum(i->tokenText.at(1)))){//where isValidNumber shoud be called
	if(isValidNum(i->tokenText)){
	  i->tokenType = TokenData::NUM;
	}else{
	  report("ERROR Malformed number: "+i->tokenText+" at line: "+to_string((long long)i->line)+": "+to_string((long long)i->charPos),4);
	  if(FORCE){
	    i->tokenType = TokenData::NUM;
	    i->tokenText = i->tokenText.at(0);
	    if(i->tokenText == ".")
	      i->tokenText += '0';
	    report("Setting value to: "+i->tokenText,0);
	  }else{
	    return false;
	  }
	}
      }else if(isReserved(i->tokenText)){//checks reserved words
	i->tokenType = TokenData::RESERVED;
	if(i->tokenText == "var")
	  i->tokenText = "__ANY__";
      }else{//assume it is meant to be an ID(varn name, function name, etc...)
	i->tokenType = TokenData::IDENT;
      }      
    }
  }
  report("Sucsess scrubbing",-2);
  return true;
}

//pushes litteral onto the vector with some extra checks
void addLitteral(TokenData i, vector<TokenData>* tokens){//should take refrence to TokenData or take info about it
  //check if litteral is a number
  if(isNum(i.tokenText.at(0)) || (i.tokenText.at(0) == '.' && isNum(i.tokenText.at(1)))){//where isValidNumber shoud be called
    if(isValidNum(i.tokenText)){
      i.tokenType = TokenData::NUM;
    }else{
      report("ERROR Malformed number: "+i.tokenText+" at line: "+to_string((long long)i.line)+": "+to_string((long long)i.charPos),4);
      if(FORCE){
	i.tokenType = TokenData::NUM;
	i.tokenText = i.tokenText.at(0);
	if(i.tokenText == ".")//make . into .0
	  i.tokenText += '0';
	report("Setting value to: "+i.tokenText,0);
      }else{
	//return false;
	exit(5);
      }
    }
  }else if(isReserved(i.tokenText)){//checks reserved words
    i.tokenType = TokenData::RESERVED;
    if(i.tokenText == "var")
      i.tokenText = "__ANY__";
  }else{//assume it is meant to be an ID(varn name, function name, etc...)
    i.tokenType = TokenData::IDENT;
    for(unsigned it = 0;it<definedAliases.size();it++){
      if(i.tokenText == definedAliases[it].first)
	i.tokenText = definedAliases[it].second;
    }
  }
  tokens->push_back(i);//would like to use emplace
}

//checks if a number is properly formatted
bool isValidNum(const string& text){//currently only checks if the number has two or more '.'
  bool dot = false;
  char C;
  for(int i = 0; i< text.length(); i++){
    C = text.at(i);
    if(isNum(C)){
      //do nothing
    }else if(C == '.'){
      if(dot)
	return false;
      dot = true;
    }else{
      return false;
    }
  }
  //check if number is valid
  //for some reason regex isnt working
  return true;
}

//checks if a word is reserved as defined in Operators.h
bool isReserved(const string& text){
  for(int i = 0; i < sizeof(RESERVED)/sizeof(RESERVED[0]); i++)
    if(RESERVED[i] == (text))
      return true;
  return false;
}

//sets up the classifications of all chars
void CharClassifier::init(){
  TypeNames[0] = "WHITESPACE";
  TypeNames[1] = "LINE_BREAK";
  TypeNames[2] = "LETTER";
  TypeNames[3] = "DIGIT";
  TypeNames[4] = "OPERATOR";
  TypeNames[5] = "STRING_QUOTE";
  TypeNames[6] = "SINGLE_LINE_COMMENT";
  TypeNames[7] = "MULTI_LINE_COMMENT_START";
  TypeNames[8] = "MULTI_LINE_COMMENT_END";
  TypeNames[9] = "OTHER";
  TypeNames[10] = "INVALID";

  for(unsigned char C = 0; C<255;C++){//currently only ASCII works. If i was a better programmer I would add unicode support, UTC-8?
    if((C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z')||C == '_')
      tk[C] = LETTER;
    else if(C == ' ' || C == '\t' || C == '\n')
      tk[C] = WHITESPACE;
    else if(isOp(C))
      tk[C] = OPERATOR;
    else if(C <= 31 || C == 127)//controle chars in ASCII
      tk[C] = INVALID;
    else if(isNum(C))
      tk[C] = DIGIT;
    else if (C == '"')
      tk[C] = STRING_QUOTE;
    else if (C == ';')
      tk[C] = LINE_BREAK;
    else
      tk[C] = OTHER;
  }
}

//checks if a char is an operator as defined in Operators.h
bool isOp(const char data){
  for(int i = 0; i < sizeof(OPS)/sizeof(OPS[0]); i++)
    if(data == OPS[i])
      return true;
  return false;
}

//cheks if a char is a digit 0-9
bool isNum(const char data){
  if(data >= '0' && data <= '9')
    return true;
  return false;
}

void clearLexertokens(vector<TokenData>* tokens){
  for(int i = 0; i< tokens->size(); i++){
    //tokens->at(i).~TokenData();
  }
  tokens->clear();
  //delete tokens;
}

//--------------------------------Import functions-------------------------------------//

inline bool fileExists(const string& name) {//https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
  struct stat buffer;
  return stat(name.c_str(), &buffer) == 0;
}

void checkControleLine(const string& line, const int lineNumber, vector<TokenData>* tokens){
  report("Line command: "+line, -2);
  auto arr = split(line, " ");
  if(arr.size() < 2){
    report("WARNING:Unrecognised command(1): "+line, 2);
    tokens->push_back(TokenData(line, lineNumber, line.length(), TokenData::LINE_COMMENT));
    return;
  }
  string data = "";
  for(int i = 1; i<arr.size()-1; i++)
    data+=arr[i]+" ";
  data += arr[arr.size()-1];
  if(arr[0] == "FILE"){
    tokens->push_back(TokenData(data, lineNumber, data.length(), TokenData::FILE_CHANGE));
  }else if(arr[0] == "import"){
    parseImport(data, tokens, lineNumber);
  }else{
    report("WARNING:Unrecognised command line: "+line, 2);
    tokens->push_back(TokenData(line, lineNumber, line.length(), TokenData::LINE_COMMENT));//if its nothing else, just comment it out, or could error
  }
}

void parseImport(const string& name, vector<TokenData>* tokens, unsigned lineNumber){
  string pathToFile;
  bool isCpp;
  report("Trying to import file: "+name, -2);
  if(fileExists(name)){
    pathToFile = name;
  }else if(name.substr(0,8) == "https://"){//only allow https
    pathToFile = "downloadedImport/"+name.substr(name.find_last_of("/")+1);//should stop when get to '?'
    if(fileExists(pathToFile))
      int prv = system((string("rm "+pathToFile)).c_str());//remove any prevoius version of the file
    if(!download(name)){
      report("Failed to download file: "+name,4);
    }
  }else{
    report("Unkown import type, or file doesn't exist: "+name,4);
    exit(12);
  }
  //check if file has already been included
  bool uniqueFile = true;
  for(vector<string>::iterator itt = importedFiles.begin(); uniqueFile && itt!= importedFiles.end();itt++)
    if(pathToFile == (*itt))
      uniqueFile = false;
  if(!uniqueFile){
    report("File "+pathToFile+" has already been imported. It is being skipped", 0);
    return;
  }
  importedFiles.push_back(pathToFile);
  string ext = pathToFile.substr(pathToFile.find_last_of('.'));
  if(ext == ".cpp" || ext == ".h" || ext == ".c" || ext == ".hpp"){
    isCpp = true;
  }else if(ext == ".tw"){
    isCpp = false;
  }else{
    report("Unkown file type: "+ext+" on import: "+name,4);
    exit(13);
  }
  if(isCpp){
    //TODO
    /*I think i need to make a rough C++ parser to find functions+parameters, would be done in 2Parse.cpp*/
    tokens->push_back(TokenData("import", lineNumber, 6, TokenData::IMPORT));
    tokens->push_back(TokenData(pathToFile, lineNumber, pathToFile.length(), TokenData::IMPORT));
    //exit(14);
  }else{
    string importFileText = string("#FILE ")+name.substr(name.find_last_of("/")+1);
    if(!readFile(pathToFile.c_str(),&importFileText)){
      report("Error reading file: "+pathToFile,4);
      exit(11);
    }
    if(!Lexer(&importFileText,tokens)){
      report("Lexer failed, terminating program", 4);
      exit(10);      //cant exit gracfully
    }else{
      //remove EOF added to end of tokens
      tokens->erase(tokens->end()-1);
      tokens->erase(tokens->end()-1);
    }
  }
}


bool download(const string& URL){//TODO add other options then wget (curl, windows options, maybe jsut use a socket?)
  report("Attempting to download file...", -2);
  string command = "cd "+pathToDownloadedImports+" && wget -q --https-only "+URL;//+" && cd ..";//TODO fix directory pathing
  if(int compileStatus = system(command.c_str())){
    return false;
  }
  return true;
}
