/*
 *  Parser.h
 *  V2
 *  Created on: Jul 4, 2018
 *  Rewritten: 9/3/18
 *  Author: Dakota
 */


#ifndef PARSER_H_2
#define PARSER_H_2

#include <vector>
#include <algorithm>
#include <exception>
#include <stdexcept>

#include "Token.h"
#include "utils/Twine_Utils.h"
#include "parserNodes.h"
#include "Operators.h"
#include "Scope.h"
//#include "resolver.cpp"--found at bottom

using namespace std;

vector<TokenData> * tokenList;          //list of all tokens provided by Lexer
vector<TokenData>::iterator sym;        //current symbol
vector<TokenData>::iterator lastSym;    //previous symbol
vector<TokenData>::iterator twoSymbAgo; //symbol from 2 symbols ago
int errors = 0;
int expectedIndents = 0;


Funk * currentFunk;
object * currentClass;//alteration
vector<Funk *> TwineFunks = (vector<Funk *>());//This stores built-in functions after they are called once
vector<object *> TwineObjects = vector<object *>();
vector<TokenData> currentLine = vector<TokenData>();//stores token used to make up current line for debugging
string currentParsingFile;

bool hitEnd = false;
bool currentLineOn = false;
//resolver
//able to concatenate atoms
void addAtoms(bigAtom * root,bigAtom * additive);

//TOP LEVEL//
bool ParseTwo(vector<TokenData>* tokens, /*vars*/scope*  varsPassedIn = new scope(), prog * p = new prog(), bool interpMode = false);
bool preScan();
void parseTop();

var parseVar(bool takeName = true);

void parseBlock(Block* b, bool pushBackVarScope = true);
void parseLine(parseNode* PN);

void addAutoFunctions(object * obj);
void addEquals(object * obj);
void addNotEquals(object * obj);
void addString(object * obj);
void addStaticString(object * obj);
void addDefualtConstructor(object * obj);

void parseClassDec(object * obj);
void parseClassPre(object * obj);
void parseClass(object * obj);

void resolveExpression(expression3 * EXP, string targetType = "");

expression3 * parseList(const vector<unsigned long>& lst, const string& type, unsigned depth=0, expression3 * exp= new expression3());

expression3 * parseExpression(const string targetType = "", const vector<unsigned long> arr = {}, bool allowAssign = true);

expression3 * parseExpression(const var& v, bool allowAssign = true){return parseExpression(v.type, v.arr, allowAssign);}

void checkReturn(Funk * F);

expression *  split(expression * EXP, int pos);
void resolveExpression(expression * EXP, string targetType = "");
void parseExpression(expression * EXP, string targetType = "");

string convertedType(string type);
//void parseAtom(atom* at = new atom(), string targetType = "", string base = "", bool baseStatic = true);
bool parseAtom(atom * a, string base, bool baseStatic);

void parseIF(IF* IF);
void parseThrow(throw_e * e);
void parseWHILE(WHILE* WHILE);
WHILE* parseDoWHILE();
void parseFOR(FOR2* FOR);
void parseForIn(FOR2 * F);
void parseNormalFOR(FOR2 * F);

var* getMemberVar(const string& classType, const string& VarName, bool STATIC = false);

Funk * functionsToFunk(functions func);
object * nameSpaceToObject(nameSpace n);
object* getClass(string name);
branch * getBranchType(string name);
Funk * getFunk(string name,string Class = "");
Funk * applyTemplate(Funk * f, object * obj);
bool removeFunk(string name, string Class = "");
bool removeClass(string name);

void parseFunk(Funk* F, bool doKW = true);
void parseFunkKW(Funk* FUNK);


void parseKW(parseNode* PN);
void parseIdent(parseNode * pn);
var * parseClassVar();
//void parseVarAssign(varAssign * va, bool anyOP = true, string name = "", bool constant = false);//TODO
void parseVarAssign(varAssign * va, bool constant, const string type);
void parseFunkCallParameters(funkCall * fc, Funk * F);
void parseFunkCall(funkCall * fc, string name = "", string base = "");

void cleanUp();

//HELPERS//
bool isCallable(string a);
bool peek(TokenData::Type sym);
bool peek(string s);
string peek();
bool accept(const string& sym);
bool accept(TokenData::Type sym);
bool expect(const string& sym, bool allowForce = true);
bool expect(TokenData::Type sym, bool allowForce = true);
bool shouldThread(const Funk* i_am_a_funkky_pointer);

void nextSym();

void forceAdd(const string& newSym);
void forceSkip();

//void error(string msg, bool passable = false);
//void warn(string msg);

int indent = 0;//down here cuz it goes with debug
void debug(string s);
//--------------------------------------------------------------------------------

void setUpUserMain(prog * Prog){
    Funk * main_funk = new Funk();
    main_funk->name = "__userMain__";
    main_funk->returnType = var("int");
    var param;
    param.type = "__ANY__";
    param.name="args";
    param.reference = true;
    main_funk->parameters.push_back(param);
    //Prog->functions.push_back(main_funk);
    Prog->functions.insert(Prog->functions.begin(), main_funk);
}

void analizeProg(prog * Prog){
  cout<<"Total length: "<<Prog->functions.size()<<endl;
  for(int i = 0; i<Prog->functions.size();i++){cout<<"Funk #: "<<i<<" "<<Prog->functions.at(i)->name<<" with size: "<<Prog->functions.at(i)->funkBlock->Lines.size()<<endl;}
}

bool isAlreadyGlobal(prog * Prog, var * Var){
  for(int i = 0; i < Prog->globalVars.size(); i++){
    if(Prog->globalVars.at(i)->name == Var->name)
      return true;
  }
  return false;
}

void matchEnd(int indent = 0){
  bool ate;
  do{
    ate = false;
    if(accept("{")){
      indent++;
      ate = true;
    }
    if(accept("}")){
      indent--;
      ate = true;
    }
    if(indent && !ate)//check for EOF?
      nextSym();

    if(peek(TokenData::END_OF_FILE) && indent){
      cout<<"indent: "<<indent<<endl;
      error("Missing closing '}'", false);

    }
  }while(indent);
}
void printSyms(){
  cout<<"Sym: "<<sym->tokenText<<endl;
  cout<<"lastSym: "<<lastSym->tokenText<<endl;
  cout<<"twoSymbAgo: "<<twoSymbAgo->tokenText<<endl;
  if(sym->tokenType != TokenData::END_OF_FILE)
    cout<<"Next sym: "<<(sym+1)->tokenText<<endl;
}
expression3 * stringToExp(string in){
  expression3 * ret = new expression3();
  atom * newAtom = new atom();
  bigAtom * bAtom = new bigAtom();
  bAtom->type = bigAtom::ATOM;
  bAtom->a=newAtom;
  ret->bigAtoms->push_back(bAtom);
  newAtom ->type = "lit";
  newAtom->literalValue = new string(in);
  return ret;
}

void setIterators(){
  sym = tokenList->begin();
  lastSym = sym;
  twoSymbAgo = lastSym;//D.A.
  if(sym->tokenType == TokenData::FILE_CHANGE)
    currentParsingFile = sym->tokenText;
  if(sym->tokenType == TokenData::BLOCK_COMMENT || sym->tokenType == TokenData::LINE_COMMENT || sym->tokenType == TokenData::LINE_END
     || sym->tokenType == TokenData::FILE_CHANGE)
    nextSym();
}
bool builtInType(string type){
  for(int i = 0; i<sizeof nameSpaces / sizeof nameSpaces[0]; i++)
    if(nameSpaces[i].name == type)
      return true;
  return false;
}
bool isType(){
  //check built in types //TODO check from built in classes
  /*for(int i = 0; i < sizeof(VARYTPE)/sizeof(VARYTPE[0]);i++){
    if(VARYTPE[i] == sym->tokenText)
      return true;
      }*/
  for(int i = 0; i < sizeof(nameSpaces)/sizeof(nameSpaces[0]);i++){//don't tell me sizeof isn't a function
    if(nameSpaces[i].name == sym->tokenText)
      return true;
  }
  //check classes
  for(int i = 0; i < Prog->classes.size(); i++){
    if(Prog->classes.at(i)->name == sym->tokenText)
      return true;
  }

  //check CPP classes
  for(int i = 0; i < TwineObjects.size(); i++){
    if(TwineObjects.at(i)->name == sym->tokenText)
      return true;
  }
  
  return sym->tokenText == "void";//isType((sym->tokenText));
}
bool acceptType(){
  if(isType()){
    debug("Eating ");
    nextSym();
    return true;
  }
  return false;
}
bool isNumOP(){
  for(int i = 0; i<sizeof(num_OPS)/sizeof(num_OPS[0]);i++)
    if(accept(string(num_OPS).substr(i,1)))
      return true;
  return false;
}
bool isValidExpOP(bool allowAssign = false){
  for(int i = 0; i<sizeof(Operators)/sizeof(Operators[0]);i++){
    if((allowAssign || !Operators[i].assignment) && !(Operators[i].OP == "[" || Operators[i].OP == "]") && accept(Operators[i].OP))//dont allow assignment OPs for normal Exp parsing(only valid for varAssign)
      return true;
  }
  return false;
}
bool isAssignment(const string& OP){
  for(int i = 0; i<sizeof(Operators)/sizeof(Operators[0]);i++){
    if(Operators[i].assignment && Operators[i].OP == OP)
      return true;
  }
  return false;
}
bool isBoolOP(const string& OP){
  for(int i = 0; i<sizeof(bool_OPS)/sizeof(bool_OPS[0]);i++)
    if(OP == bool_OPS[i])
      return true;
  return false;
}
bool isBoolOP(){
  for(int i = 0; i<sizeof(bool_OPS)/sizeof(bool_OPS[0]);i++)
    if(accept(bool_OPS[i]))
      return true;
  return false;
}
bool isValue(){//TODO: add support for expressions and negatives
  if(acceptType())
    return true;

  if(accept("true") || accept("false"))
    return true;

  if(accept("-")){
    if(accept(TokenData::NUM))
      return true;
    if(getVARType(sym->tokenText) != "DNE" && accept(TokenData::IDENT))
      return true;
    error("Invalid number assignment");
    return false;
  }
  return (getVARType(sym->tokenText) != "DNE" && accept(TokenData::IDENT));
  /*if(isType(sym->tokenText)){
    nextSym();D.A
    return true;}*/
}

void cleanUp(){
  for(int i = 0;i<TwineFunks.size(); i++){
    //cout<<"Cleaning up "<<i<<endl;
    TwineFunks.at(i)->~Funk();
    //cout<<"deleting"<<endl;
    //delete TwineFunks.at(i);
  }
  //cout<<"clear"<<endl;
  TwineFunks.clear();
}


expression3 * intToExp3(int number){
  expression3 * ret = new expression3();
  ret->cast = "";
  
  atom * holder = new atom();
  holder->type = "lit";
  holder->literalValue = new string(to_string(number));
  
  ret->bigAtoms->push_back(new bigAtom(holder));
  return ret;
}

expression3 * strToExp3(string str){
  expression3 * ret = new expression3();
  ret->cast = "";

  atom * holder = new atom();
  holder->type = "lit";
  holder->literalValue = new string("\""+str+"\"");

  ret->bigAtoms->push_back(new bigAtom(holder));
  return ret;
}

expression3* atomToExp3(atom * a){
  expression3* ret = new expression3();
  ret->cast = "";
  ret->bigAtoms->push_back(new bigAtom(a));
  return ret;
}

atom * exp3ToAtom(expression3 * exp){
  atom * ret = new atom();
  ret->type = "exp3";
  ret->exp3 = exp;
  return ret;
}

bigAtom * exp3ToBigAtom(expression3 * exp){
  bigAtom * ret = new bigAtom();
  ret->type = bigAtom::ATOM;
  ret->a = exp3ToAtom(exp);
  return ret;
}

bigAtom * OPToBigAtom(const string& str){
  bigAtom * ret = new bigAtom();
  ret->type = bigAtom::OP;
  ret->op = str;
  return ret;
}

/*expression2 * exp3toexp2Sized(expression3 * expIn, int start, int end){
  expression3 * exp3 = new expression3();
  for(int i = start; i < end; i++){
    exp3->bigAtoms->emplace_back(expIn->bigAtoms->at(i));
  }
  expression2 * exp2 = new expression2();
  exp2->leftAtom = exp3ToAtom(exp3);
  //exp2->leftIsExp = true;
  exp2->cast = "";
  return exp2;
  }*/


//resolver

#include "Linter.h"
#include "CppParser.h"
//#include "codeTmp.cpp"
#include "branchParse.cpp"

expression2 * resolveExpression2(expression3 * expIn, string targetType);
//expression2 * resolveExpressionHelper(expression3 * expIn, string targetType, int start, int end, string typeCalledOn, bool isStatic);
expression2 * resolveExpressionHelper(expression3 * expIn, string targetType, int start, int end, string typeCalledOn = "", bool isStatic = false, vector<unsigned long> arr = {});
expression2 * resolveSingle(atom * a, string targetType, vector<unsigned long> arr = {});
void resolveAtom(atom * a);
expression2 * resolveAtoms(atom * left, atom * right, string OP);
expression2 * resolveExpressions(expression2 * left, expression2 * right, string OP);
void applyCast(expression2 * EXP, string targetType);
void applyCast(atom * a, string targetType);//??

void printExp(expression2 * EXP);
void printExp(expression3 * EXP, int start=0, int end=-1);
void printEXP(expression3 * EXP);
#endif //PARSER_H_2
