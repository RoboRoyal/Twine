

/*
 *4/27/19
 *used to parse CPP/H files
 *Extracts function names, return types, # of parameters, parameter types, and if they have defualt values
 */


//simple function:
//RETURN_TYPE NAME([PARAMETERS])
#include "Lexer.h"
#include "2Parser.h"

void parseCppFile(const string& name);
vector<TokenData>* getTokenisedFile(const string& fileName);
void parseCppTop();
bool allow(const string& text);
var * parseCppParameterVar();
Funk * parseCppFunction();
void parseCppFile(const string& pathToFile);
object * parseCppClass();
bool checkIfFunc();
void matchLessSym();


//parse var
//parse type

vector<TokenData>* getTokenisedFile(const string& fileName){
  //set lexer to right settings
  //dont save comments/whitespaces, yes scrub, 
  /*string importFileText = "";
  if(!readFile(fileName.c_str(),&importFileText)){
    report("Error reading file: "+pathToFile,4);
    exit(21);
    }*/
  string importFileText = "";
  bool read = readFile(fileName.c_str(), &importFileText);
  vector<TokenData>* tokens = new vector<TokenData>;
  if(!read)
    return tokens;
  if(!Lexer(&importFileText, tokens)){//I know my lexer isn't C++ compatable, but its all i have on hand
    report("Lexer failed, terminating program", 4);
    exit(20);      //cant exit gracfully
  }else{
    return tokens; 
  }
  return NULL;//should never get here but if we do make sure we crash
}

void parseCppTop(){
  while(!accept(TokenData::END_OF_FILE)){
    bool changed = false;
    allow(";");
    if(peek("class")){
      changed = true;
      object * tmp = parseCppClass();
      if(tmp)
	TwineObjects.push_back(tmp);
    }else{
      Funk * tmp = parseCppFunction();
      if(tmp){
	debug("Adding funk "+tmp->name+" ");
	TwineFunks.push_back(tmp);
	changed = true;
      }
    }
    if(!changed)//prevent inf loops
      nextSym();
  }
}

bool allow(const string& text){//may move sym forward, but always returns true
  accept(text);
  return true;
}

var * parseCppParameterVar(){
  debug("parseCppParameterVar()");
  var * a = new var();
  if(accept("const"))
    a->constant = true;
  allow("std::");
  if(acceptType() || accept(TokenData::IDENT))//type is for regular types, IDENT is for classes not parsed
     ;
  a->type = lastSym->tokenText;
  if(accept("&")){
    a->reference = true;
  }else if(accept("*"))
    a->ptr = true;
  accept(TokenData::IDENT);
  a->name = lastSym->tokenText;
  if(accept("=")){
  //doesnt need to be anything as long as startingValue != NULL (could just set it to 1?)
  //need to find end of expression, but i dont want to actually parse a Cpp Expression
  //this could be done by looking for a comma, or an end ) of a ;
    //expression3 * tmp = new expression3(); 
    a->startingValue =     parseExpression();//TODO
  }
  if(a->name == "" || a->type == "" || a->name == a->type){
    report("Invalid C parameter found: "+a->name+" with type: "+a->type, -2);
    exit(50);
  }
  report("Found C parameter: "+a->name+" with type: "+a->type, -2);
  debug("parseCppParameterVar() done");
  return a;
}

void parseCppParameters(Funk * cppFunk){
  debug("parseCppParameters()");
  while(!accept(")")){
    cppFunk->parameters.push_back(*parseCppParameterVar());
    accept(",");
  }
  debug("parseCppParameters() done");
}

Funk* parseCppFunction(){
  //debug("parseCppFunction()");
  string name, returnType;
  Funk * cppFunk = NULL;
  if (allow("const") && allow("inline") && acceptType()){
    returnType = lastSym->tokenText;
    if(allow("*") && accept(TokenData::IDENT)){
      name = lastSym->tokenText;
      if(allow("const") && accept("(") && name != "main"){
        //found function
	report("Found C function: "+name+" with return type: "+returnType, -2);
        cppFunk = new Funk;
        cppFunk->name = name;
        cppFunk->returnType = returnType;
        parseCppParameters(cppFunk);
	//TwineFunks.push_back(cppFunk);
	matchEnd(0);
      }
    }
    allow("const");
    allow("noexcept");//not really needed as it will be ignored anyways
  }
  //debug("parseCppFunction() done");
  return cppFunk;;
}

void parseCppFile(const string& pathToFile){
  vector<TokenData>* tokens = getTokenisedFile(pathToFile);
  if(tokens == NULL || tokens->size() < 3)
    return;
  
  vector<TokenData>::iterator originalSym = sym;//save old syms
  vector<TokenData>::iterator originalLastSym = lastSym;
  vector<TokenData>::iterator originalTwoSymAgo = twoSymbAgo;

  sym = tokens->begin();
  parseCppTop();
  include * im = new include();
  im->cTypeInclude = true;
  im->fileName = pathToFile;
  Prog->globalIncludes.push_back(im);
  
  sym = originalSym;//restore iterators
  lastSym = originalLastSym;
  twoSymbAgo = originalTwoSymAgo;
}


object * parseCppClass(){
  debug("parseCppClass()");
  expect("class");
  object * obj = new object;
  expect(TokenData::IDENT);
  obj->name = lastSym->tokenText;
  if(accept(":") && allow("public") && accept(TokenData::IDENT)){
    obj->extends = lastSym->tokenText;
  }
  expect("{");
  while(!accept("}")){
    accept("public"); accept("private"); accept("protected"); accept(":");
    if(accept(obj->name)){//constructor
      expect("(");
      Funk * tmp = new Funk();
      tmp->name = obj->name;
      tmp->returnType = obj->name;
      parseCppParameters(tmp);
      //obj->memberFunks.push_back(tmp);
      TwineFunks.push_back(tmp);
      matchEnd();
    }else if(checkIfFunc()){
      Funk * tmp = parseCppFunction();
      if(tmp)
	obj->memberFunks.push_back(tmp);
    }else{
      obj->memberVars.push_back(parseCppParameterVar());
      while(!accept(";"))
	nextSym();
    }
  }
  debug("parseCppClass() done");
  return obj;
}//TODO


bool checkIfFunc(){
  vector<TokenData>::iterator origonal = sym;
  vector<TokenData>::iterator origonalLast = lastSym;
  allow("const"); acceptType(); accept(TokenData::IDENT);
  bool ret = accept("(");
  sym = origonal;
  lastSym = origonalLast;
  return ret;
}

void matchLessSym(){
  unsigned sharkCount = 1;
  while(!sharkCount){
    nextSym();//cant have 2+ < in a row
    sharkCount += accept("<");
    nextSym();//cant have <>
    while(accept(">"))//can have multiple >> in a row
      sharkCount -= 1;
  }
}

void parseType(){//func or var
  var tmp = var();
  if(accept("const")) tmp.constant = true;
  nextSym();
  tmp.type = lastSym->tokenText;
  if(accept("<"))
     matchLessSym();
  if(accept("*")) tmp.ptr = true;
  nextSym();
  tmp.name = lastSym->tokenText;
  if(accept("&")) tmp.reference = true;
}

