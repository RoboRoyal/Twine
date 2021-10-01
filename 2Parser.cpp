
/*
 * 2Parser.cpp
 * Main parser file. Takes a list of tokens and creats a custom prog object.
 */

#include "2Parser.h"

//class Parse2{

var parseVar(bool takeName){//takeName = true
  var v = var();
  v.constant = accept("const");
  v.STATIC = accept("static");
  if(acceptType()){
    v.type = lastSym->tokenText;
  }else{
    v.type = "__ANY__";
    if(lint && currentLineOn) checkTypeAllow(v.name);//check for currentLineOn so it only prints lint msg durring normal parsing, not durring pre-scan
  }
  while(accept("[")){
    if(accept(TokenData::NUM)){//defined size //TODO allow constants
      long listSize= stoi(lastSym->tokenText);//int, long, unsigned?
      if(listSize <= 0)
        error("List size must be >0");
      v.arr.push_back(listSize);
    }else{//undefined size
      v.arr.push_back(0);
    }
    expect("]");
    v.isList = true;
  }
  if(accept("&")) v.reference = true;
  if(takeName){
    expect(TokenData::IDENT);//get name
    v.name = lastSym->tokenText;
  }
  v.lineDefinedOn = sym->line;
  /*
    if(accept("=")){
    v.startingValue = parseExpression(V);
    }
   */
  return v;
}

//sets up parsing conditions to enable parseList to work
vector<unsigned long> parseUncertainList(expression3 * exp, const string type = "__ANY__", const bool isCon = false){
  vector<unsigned long> arr = vector<unsigned long>();
  bool needComma = false;
  unsigned long size = 0;
  const vector<TokenData>::iterator tmpHold = sym;//save
  while(accept("{")){
    arr.push_back(0);
  }
  sym = tmpHold;//revert

  exp = parseList(arr, type, 0, exp);
  return arr;
}
expression3 * parseList(const vector<unsigned long>& lst, const string& type, unsigned depth, expression3 * exp){
  debug("parseList()");
  expect("{");
  exp->cast = "list";
  if(lst[depth] == 0){
    do{
      if(depth + 1== lst.size())
	exp->bigAtoms->push_back(exp3ToBigAtom(parseExpression(type)));
      else
	exp->bigAtoms->push_back(exp3ToBigAtom(parseList(lst, type, depth+1)));
      if(!peek("}")){
	expect(",");
	exp->bigAtoms->push_back(OPToBigAtom(","));
      }
    }while(!accept("}"));
    //accept(",");//allow trailing comma
  }else{
    for(unsigned itt = 0;itt<lst[depth];itt++){
      if(accept("}")) error("Missing elements in list, recieved "+to_string(itt)+"/"+to_string(lst[depth]));
      if(depth + 1== lst.size()){
	exp->bigAtoms->push_back(exp3ToBigAtom(parseExpression(type)));
      }else{
	exp->bigAtoms->push_back(exp3ToBigAtom(parseList(lst, type, depth+1)));
      }
      if(itt+1 != lst[depth]){
	expect(",");
	exp->bigAtoms->push_back(OPToBigAtom(","));
      }
    }
    accept(",");//allow trailing comma
    expect("}");
  }
  debug("parseList() done");
  return exp;
}

//dummy is called when an unimplimented feature is used
bool dummy(){return true;}

//function called to parse tokens. Sets up scopes, global varriables directs prescam, then actual scan
bool ParseTwo(vector<TokenData>* tokens, /*vars*/scope * varsPassedIn, prog * p, bool interpM){
    report("Parsing file",0);

    if(interpM && currentParsingFile == "")
      currentParsingFile = "Interpreted-CL";
    
    //set global vars
    interpMode = interpM;
    tokenList = tokens;
    currentVars = varsPassedIn;//?????
    localVars = new vector<scope *>;

    if(varsPassedIn != NULL){
      globalVars = varsPassedIn;
    }else{
      globalVars = new scope();
    }
    
    currentLineOn = false;
    Prog = p;
    currentClass = NULL;
    
    setIterators();
    
    //Set up main function
    if(!interpM)
      setUpUserMain(Prog);
    currentFunk = Prog->functions.at(0);

    //pre-scan, get classes/function defs
    if(!preScan()){
      error("Prescan failed");
      return false;
    }
      
    //set up things for regular parsing
    currentLineOn = true;//this defines if on prescan or regual scan
    setIterators();
    
    //set up built in varriables
    initDefualtVars();//--implimented in Scope.h

    //parse file
    try{
      parseTop();
    }catch(const exception& e){
      report("Error parsing",4);
      report(e.what(),4);
      return false;
    }
    if(!interpMode)
      //TODO clean up scope and tokens maybe
      dummy();

    //lint last part of file
    if(lint){
      checkProg(Prog);
      finalLint();
    }
    if(reportingLevel < -1)
      cout<<Prog->toString();
    if(varsPassedIn == NULL)//delete vars if we made them
      cleanScopes();
    
    if(numberOfScopes()>1 && !errors)
      throw invalid_argument("Logic error, still scopes left");
    report("Success parsing file",0);
    return true;
  }

/*scans the file and looks for all defined functions and classes*/
//This could be done away with by using a smarter parser, catagorizing calls,
//then parsing them after the funcition gets defined
bool preScan(){
  debug("prescan()");
  while(!accept(TokenData::END_OF_FILE)){//loop throught whole file
    if(accept("func")){//find all functions

      Funk * skell = new Funk();
      parseFunkKW(skell);//get name/returntype/parameters
      
      if(getFunk(skell->name) != NULL){
	if(getFlag("FUNCTION_REDEFINITION") || interpMode){
	  removeFunk(skell->name);
	  warn("Redefinition of function '"+skell->name+"'");
	}else
	  error("Redefinition of function '"+skell->name+"'", true);
      }
      
      Prog->functions.push_back(skell);
      
    }else if(accept("class")){//TODO object?
      object * obj = new object;
      //get name and extends
      parseClassDec(obj);
      
      //check if it is a new class
      if(getClass(obj->name) != NULL && !getFlag("CLASS_REDEFINITIONS")){
	error("Redefinition of class "+obj->name, true);
	//TODO delete old implimentation
	removeClass(obj->name);
      }
      //add it to Prog
      Prog->classes.push_back(obj);
      
      //prescan it go get function defs and member vars
      parseClassPre(obj);
    }else if(accept("branch")){
      branch * B = new branch;
      parseBranchDec(B);
      if(getClass(B->name) != NULL && !getFlag("CLASS_REDEFINITIONS")){
	error("Redefinition of branch "+B->name, true);
	//TODO delete old implimentation
      }
      //add it to Prog
      Prog->classes.push_back(B);
      Prog->branches.push_back(B);
      
      //prescan it go get function defs and member vars
      parseBranchPre(B);
      
    }else if(accept("import")){//handle imports
      if(interpMode) error("Can not import non-twine files when in interpreting mode");
      accept(TokenData::IMPORT) || expect(TokenData::IMPORT); 
      parseCppFile(lastSym->tokenText);
    }else if(accept(TokenData::FILE_CHANGE)){//track current file to make error messages better
      currentParsingFile = lastSym->tokenText;
    }else{//progress through file
      nextSym();
    }
  }
  debug("prescan() done");
  return true;
}

/*parses name, privacy and extention*/
void parseClassDec(object * obj){
  debug("parseClassDec()");
  expect(TokenData::IDENT);
  obj->name = lastSym->tokenText;//public/private extends?
  if(accept("extends") || accept("::")){
    expect(TokenData::IDENT);
    obj->extends = lastSym->tokenText;
  }else{
    obj->extends = "Object";
  }
  debug("parseClassDec() done");
}

//pre scan class parseses function KW, adds blank function to class
//also parses class vars
//parseClass proper will fill in functions

/*skeleton parse of class, getting member vars and functions*/
void parseClassPre(object * obj){
  debug("parseClassPre()");
  expect("{");
  pushScope();
  currentClass = obj;
  while(!accept("}")){
    if(accept("func") || accept("def")){
      Funk * F = new Funk;
      parseFunkKW(F);
      if(F->name == obj->name){
	if(obj->constructors.size())//currently only support 1 constructor
	  error("Redefinition of class constructor: "+obj->name);
	F->returnType = var(obj->name);
	obj->constructors.push_back(F);
      }else{
	if(F->name == "toString"){
	  F->returnType = var("string");
	}else if(F->name == "toNum"){
	  F->returnType = var("double");
	}
	obj->memberFunks.push_back(F);
      }
      if(!peek("{"))
	error("Missing block for function '"+F->name+"' in class '"+obj->name+"'");
	 matchEnd();//go to end of function
    }else{
      obj->memberVars.push_back(parseClassVar());
      //obj->memberVars.push_back(new var(parseVar()));
    }
    if(sym->tokenType == TokenData::END_OF_FILE)
      error("Missing close '}' for class "+obj->name, false);
  }
  //TODO add unimplimented OPs
  addAutoFunctions(obj);
  currentClass = NULL;
  pullScope();
  debug("parseClassPre() done");
}
void addAutoFunctions(object * obj){//TODO check if these are used/needed
  if(!getFlag("AUTO_FUNCTIONS"))
    return;
  bool hasEqual = false, hasNotEqual=false, hasString = false, hasDefualtConstructor = false;
  for(int i = 0; i<obj->memberFunks.size(); i++){
    if(obj->memberFunks.at(i)->name == "operator==")
      hasEqual = true;
    if(obj->memberFunks.at(i)->name == "operator!=")
      hasNotEqual = true;
    if(obj->memberFunks.at(i)->name == "toString")
      hasString = true;
    if(obj->memberFunks.at(i)->name == obj->name && obj->memberFunks.at(i)->parameters.size() == 0)
      hasDefualtConstructor = true;
  }
  if(!hasEqual)
    addEquals(obj);
  if(!hasNotEqual)
    addNotEquals(obj);
  if(!hasDefualtConstructor && false)//TODO
    addDefualtConstructor(obj);
  if(!hasString && getFlag("AUTO_TO_STRING")){
    addString(obj);
    if(getFlag("AUTO_TO_STRING_STATIC"))
      addStaticString(obj);
  }
}
void addDefualtConstructor(object * obj){
  Funk * F = new Funk;
  F->returnType = var("");
  F->name = obj->name;
  parseNode * p = new parseNode;
  p->type = "C++";
  p->data = "/*AUTO-GENERATED-CONSTRUCTOR*/";
  F->funkBlock->Lines.push_back(p);
  obj->memberFunks.push_back(F);
}
void addEquals(object * obj){//used to auto generate == operator in a class
  
  Funk * F = new Funk;
  F->constant = true;
  F->returnType = var("bool");
  F->name = "operator==";

  var a = var();
  a.type = obj->name;
  a.constant = true;
  a.reference = true;
  a.name = "ot";
  F->parameters.push_back(a);
  parseNode * p = new parseNode;
  p->type = "C++";
  if(obj->extends != "Object" && !obj->isBranch)
    p->data = "if(!this->"+obj->extends + "::operator==(ot)) return false;\n";
  for(int i = 0; i<obj->memberVars.size(); i++){
    if(!obj->memberVars.at(i)->STATIC && !obj->memberVars.at(i)->constant){
      //TODO-not use cpp code
      p->data += "if(this->"+obj->memberVars.at(i)->name+"!= ot."+obj->memberVars.at(i)->name+") return false;\n";
    }
  }
  p->data += "return true;";
  F->funkBlock->Lines.push_back(p);
  obj->memberFunks.push_back(F);
  
  Funk * G = new Funk;
  G->constant = true;
  G->returnType = var("bool");
  G->name = "operator==";
  
  var b = var();
  b.type = "__ANY__";
  b.constant = true;
  b.reference = true;
  b.name = ("a");
  G->parameters.push_back(b);
  
  parseNode * l = new parseNode;
  l->type = "C++";
  l->data = "if(typeid(*this).name() == a.type && (*("+obj->name+"*) a.ptr) == (*this)) return true; return false;";
  G->funkBlock->Lines.push_back(l);
  obj->memberFunks.push_back(G);
}
void addNotEquals(object * obj){

  Funk * F = new Funk;
  F->constant = true;
  F->returnType = var("bool");
  F->name = "operator!=";
  
  var a = var();
  a.type = obj->name;
  a.constant = true;
  a.reference = true;
  a.name = "ot";
  F->parameters.push_back(a);
  
  parseNode * p = new parseNode;
  p->type = "C++";
  if(obj->extends != "Object" && !obj->isBranch)
    p->data = "if(this->"+obj->extends + "::operator!=(ot)) return true;\n";//TODO
  for(int i = 0; i<obj->memberVars.size(); i++){//TODO call super !=
    if(!obj->memberVars.at(i)->STATIC && !obj->memberVars.at(i)->constant){
      p->data += "if(this->"+obj->memberVars.at(i)->name+"!= ot."+obj->memberVars.at(i)->name+") return true;";
    }
  }
  p->data += "return false;";
  F->funkBlock->Lines.push_back(p);
  obj->memberFunks.push_back(F);
}

void addString(object * obj){
  
  Funk * F = new Funk;
  F->constant = true;
  F->returnType = var("string");
  F->name = "toString";
  
  parseNode * p = new parseNode;
  p->type = "C++";
  p->data = "string out = string(\""+obj->name+"\")+\" @\"+to_string((long long) this)+'\\n';\n";
  if(obj->extends != "Object")
    p->data += "out+=this->"+obj->extends + "::toString()+'\\n';\n";
  for(int i = 0; i<obj->memberVars.size(); i++){
    if(!obj->memberVars.at(i)->STATIC && !obj->memberVars.at(i)->constant){
      if(obj->memberVars.at(i)->arr.size()){
	p->data += "out += string(\""+obj->memberVars.at(i)->name+"\")+\" = \"+__ANY__(this->"+obj->memberVars.at(i)->name+").toString() +'\\n';\n";
      }else if(obj->memberVars.at(i)->type == "string"){
	p->data += "out += string(\""+obj->memberVars.at(i)->name+"\")+\" = \"+this->"+obj->memberVars.at(i)->name+"+'\\n';\n";
      }else if(obj->memberVars.at(i)->type == "int" || obj->memberVars.at(i)->type == "double"){
	p->data += "out += string(\""+obj->memberVars.at(i)->name+"\")+\" = \"+to_string(this->"+obj->memberVars.at(i)->name+")+'\\n';\n";
      }else if(obj->memberVars.at(i)->type == "__ANY__"){
	p->data += "out += string(\""+obj->memberVars.at(i)->name+"\")+\" = \"+this->"+obj->memberVars.at(i)->name+".toString()+'\\n';\n";
      }else{
	p->data += "out += string(\""+obj->memberVars.at(i)->name+"\")+\" = \"+this->"+obj->memberVars.at(i)->name+".toString()+'\\n';\n";
      }
    }
  }
  p->data += "return out;";
  F->funkBlock->Lines.push_back(p);
  obj->memberFunks.push_back(F);
}

void addStaticString(object * obj){
  
  Funk * F = new Funk;
  F->STATIC = true;
  F->returnType = var("string");
  F->name = "toString";
  parseNode * p = new parseNode;
  p->type = "C++";

  p->data = "string out = string(\""+obj->name+"\");\n";
  if(obj->extends != "Object")
    p->data += "out+="+obj->extends + "::toString()+'\\n';\n";
  for(int i = 0; i<obj->memberVars.size(); i++){
    if(obj->memberVars.at(i)->STATIC && !obj->memberVars.at(i)->constant){
      if(obj->memberVars.at(i)->type == "string" || obj->memberVars.at(i)->type == "__ANY__"){
        p->data += "out += string(\""+obj->memberVars.at(i)->name+"\")+\" = \"+"+obj->memberVars.at(i)->name+"+'\\n';\n";
      }else if(obj->memberVars.at(i)->type == "int" || obj->memberVars.at(i)->type == "double"){
        p->data += "out += string(\""+obj->memberVars.at(i)->name+"\")+\" = \"+to_string("+obj->memberVars.at(i)->name+")+'\\n';\n";
      }else{
        p->data += "out += string(\""+obj->memberVars.at(i)->name+"\")+\" = \"+"+obj->memberVars.at(i)->name+".toString()+'\\n';\n";
      }
    }
  }
  p->data += "return out;";
  F->funkBlock->Lines.push_back(p);
  obj->memberFunks.push_back(F);
}
void setClassVars(object * obj){//adds member vars to current scope
  for(int i = 0; i<obj->memberVars.size(); i++){
    //setVar(obj->memberVars.at(i)->name, obj->memberVars.at(i)->type, false, obj->memberVars.at(i)->constant);
    setVar(obj->memberVars[i]);
  }
  
  if(obj->extends != "object"){//sets vars of base class
    for(int i = 0; i < Prog->classes.size(); i++){
      if(obj->extends == Prog->classes.at(i)->name)
	setClassVars(Prog->classes.at(i));
    }
  }
  
  setVar("this", obj->name);
  if(obj->extends != "")
    setVar("super", obj->extends);
}
void parseClass(object * obj){//TODO is it class or object?
  debug("parseClass()");
  currentClass = obj;
  pushScope();
  setClassVars(obj);
  pushScope();
  expect("{");
  int indent = 1;
  while(indent){//(!accept("}")){
    currentLine.clear();
    totalLines++;
    if(accept("func")){
      //parse func
      Funk tmpF = Funk();
      parseFunkKW(&tmpF);//TODO params are doubled currently
      //TODO add vars to scope?
      //pushScope();
      parseFunk(getFunk(tmpF.name, obj->name), false);//the false means to not parse FunkKW
      //TODO free memory inside tmpF
    }else{
      //parse member var
      //var * dummy = parseClassVar();//dont save vars here as pre scan call already does that
      //delete dummy;//TODO currently this parses and saves the starting value. This is iniffecent and cuasses a memory leak
      //Instead it should just skip it
      if(accept("{"))
	indent++;
      else if(accept("}"))
	indent--;
      else
	nextSym();
    }
  }
  if(lint)
    checkClassName(obj);
  pullScope();
  pullScope();
  currentClass = NULL;
  debug("parseClass() done");
}

void checkReturn(Funk * F){
  //TODO add automatic return of last defined var?
  /*
    def getNum(){
    x = input("enter number")
    x=x^2+PI
    //implicit return of x
    }
   */
  //debug("checkReturn()");
  if(F->name == F->returnType.type){//it is a constructor
    //debug("checkReturn() done");
    return;
  }
    
  if(getFlag("DEFUALT_RETURN") && F->returnType.type != "void" && (F->funkBlock->Lines.size() == 0 || F->funkBlock->Lines.back()->type != "ret")){
    //check las lines for implicet return?
    parseNode * newReturn = new parseNode();
    expression3 * returnExp = new expression3();
    atom * returnAtom = new atom();
    newReturn->type = "ret";
    returnAtom->type = "lit";
    if(F->returnType.type == "string"){
      returnAtom ->helper = var("string");
      returnAtom->literalValue = new string("\"\"");
    }else{
      returnAtom->helper = var("num");
      returnAtom->literalValue = new string("0");
    }
    returnExp->bigAtoms->push_back(new bigAtom(returnAtom));
    newReturn->theThing = returnExp;
    F->funkBlock->Lines.push_back(newReturn);
  }
  //debug("checkReturn() done");
}


void parseTop(){
  debug("parseTop()");
  //TODO have to do scopes
  while(!accept(TokenData::END_OF_FILE)){
    if(accept("func") || accept("def")){
      Funk tmpF = Funk();
      pushScope();
      parseFunkKW(&tmpF);//TODO params are doubled currently
      Funk * me = getFunk(tmpF.name, "");
      currentFunk = me;
      parseFunk(me, false);//the false means to not parse FunkKW
      currentFunk = Prog->functions.front();//return to userMain function
      //delete tmpF.funkBlock;//delete ptr automotaically made in defualt constructor, TODO should be done in deconstructor
      pullScope();
    }else if(accept("object") || accept("class")){
      object tmp = object();
      parseClassDec(&tmp);//get name of class
      parseClass(getClass(tmp.name));//get class and parse it
    }else if(accept("branch")){
      branch B = branch();
      parseBranchDec(&B);//get name of class
      parseBranch((branch *)getClass(B.name));//get class and parse it
    }else{
      parseNode * me = new parseNode();
      currentFunk = Prog->functions.front();
      parseLine(me);
      if(me->type == "varAss" && !isAlreadyGlobal(Prog, ((varAssign *)me->theThing)->VAR)){//code for globalVars--turns local var assigns in userMain to global vars
	string type = ((varAssign *)me->theThing)->VAR->type;
	if(((varAssign *)me->theThing)->VAR->constant || !(type=="int" || type=="bool" || type=="string" || type=="__ANY__" || type=="double")){
	  ((varAssign *)me->theThing)->VAR->startingValue = ((varAssign *)me->theThing)->exp3;
	  var * tmp = new var(((varAssign *)me->theThing)->VAR);
	  Prog->globalVars.push_back(tmp);
	  me->type = "EMPTY";
	  /*if(removeVar(tmp->name))
	    setVar(tmp, true);*/
	}else{
	  Prog->globalVars.push_back(new var(((varAssign *)me->theThing)->VAR));
	  ((varAssign *)me->theThing)->VAR->clear();
	  /*if(removeVar(->name))
            setVar(tmp, true);*///TODO 
	}
      }
      Prog->functions.front()->funkBlock->Lines.push_back(me);
    }
  }
  checkReturn(Prog->functions.at(0));//check return of main
  try{ debug("pareTop() done");}catch(const exception& e){cout<<"  pareTop() done(err on line)"<<endl;}//this fixes a bug that shouldnt exist
}

void parseFunk(Funk* F, bool doKW){
  debug("parseFunk("+to_string((int) doKW)+")");
  if(doKW){//Dont do KW when parsing class functions
    pushScope();//is form parseTop or parseClass, scope has already been pushed and FunkKW have been parsed as well
    parseFunkKW(F);//auto adds paramater vars to scope
  }else{
    
  }
  if(lint){
    checkFunkName(F);
    totalLines++;
  }
  //activeFunk = *F;
  currentFunk = F;
  //TODO this is redefinition?
  F->funkBlock->Lines.clear();
  parseBlock(F->funkBlock, true);
  checkReturn(F);//TODO only do if not constructor
  
  if(lint){
    checkFunk(F, F->name != F->returnType.type);
    //TODO check trailing blanks
    newLineAfterFunk(*F);
  }
  if(doKW){
    pullScope();
  }
  debug("parseFunk() done");
}
void parseFunkKW(Funk* F){//TODO lint checks happen twice here
  debug("parseFunkKW()");
  
  //get function modifiers
  if(isType()){//return type
  //if(acceptType())
    //nextSym();
    //F->returnType = var(lastSym->tokenText);
    F->returnType = parseVar(false);
  }
  if(accept("static"))
    F->STATIC = true;
  if(accept("open")){
    F->open = true;
  }else if(accept("closed"))
    F->closed = true;
  //get and set name
  if(accept(TokenData::IDENT)){//function name
    F->name = lastSym->tokenText;
    if(peek() != "(" && accept(TokenData::OPERATOR))
      F->name += lastSym->tokenText;
  }else{
    //TODO make better
    if(currentClass != NULL && F->returnType.type == currentClass->name){//class constructor
      //assuming it is constructor
      F->returnType = var("");
      F->name = currentClass->name;
    }else{
      if(peek(TokenData::RESERVED)){
	error("Invalid name for function. '"+sym->tokenText+"' is a reserved word", false);
      }else
	error("Missing name for function", false);
    }
  }
  //parse parameters
  expect("(");
  bool needComma = false;
  bool isVer = false;

  while(!accept(")")){
    if(!(accept(",") ^ needComma)){
      var a = parseVar();
      if(accept(".")){// && accept(".") && accept(".")){//poor way of doing it
	expect("."); expect(".");
	if(isVer)
	  error("Can only have one veradic(\"...\") argument per function");
	//need to set argLen TODO
	//var argLen = var(); argLen.type = "int"; argLen.name = "argLen";argLen.startingValue = intToExp3(0);F->parameters.push_back(argLen);
	a.secondaryType = "...";
	F->dynamicParamType = a.type;
	F->dynamicParamVar = a.name;
	a.type = "__ANY__";//TODO could otherwise be vector
	isVer = true;
      }
      if(a.constant && (a.type != "int" && a.type != "bool" && a.type != "double"))//if it is const, no harm in making it a reference
	a.reference = true;
      if(currentLineOn){//add var to scope if not in pre-scan
	//setVar(a.name,a.type, false, a.constant);
	setVar(a);
	if(lint) checkVarName(a.name);
      }
      if(accept("="))
	a.startingValue = parseExpression(a.type);
      
      if(!isVer)
	F->parameters.push_back(a);
      needComma = true;
    }else{
      error("Missing comma in function parameter or invalid var type '"+ lastSym->tokenText +"'", false);//TODO make skippable
    }
  }
  debug("parseFunkKW() done");
}

/* converts complicated types into only a few simple ones
   This make dealing with the diffrent data types easily*/
string convertedType(string type){
  if(type == "num"||type == "int"||type == "double"||type == "bool"){
    return "num";
  }else if(type == "stringLit"){
    return "string";
  }else{
    return type;
  }
}

/**/
string getTypeAtomHelper(atom * a){
  if(a->cast == ""){
    return a->helper.type;
  }else{
    return a->cast;
  }
}

//returns the type the exp will resolve to
string getTypeExp(expression3 * EXP){
  debug("getTypeExp()- ");
  bool strings = false, anys = false, nums = false, bools = false;
  string other = "";
  int highestPriorety = -42, pos = 0;
  for(int i = 0;i<EXP->bigAtoms->size();i++){
    if(EXP->bigAtoms->at(i)->type == bigAtom::ATOM){
      string tp = convertedType(getTypeAtomHelper(EXP->bigAtoms->at(i)->a));
      if(getTypeAtomHelper(EXP->bigAtoms->at(i)->a) == "bool")
	 tp = "bool";
      if(tp == "num"){
	nums = true;
      }else if(tp == "string"){
	strings = true;
      }else if(tp == "__ANY__"){
	anys = true;
      }else if(tp == "bool"){
	bools = true;
      }else{
	//TODO
	if(other == "")
	  other = tp;
      }
    }else if(getOP(EXP->bigAtoms->at(i)->op).priority > highestPriorety){//it must be OP
      highestPriorety = getOP(EXP->bigAtoms->at(i)->op).priority;
      pos = i;
    }
  }
  cout<<strings<<anys<<nums<<bools<<endl;
  if(pos && getOP(EXP->bigAtoms->at(pos)->op).returnsBool)
    return "bool";
  if(other != ""){
    return other;
  }else if(anys){
    return "__ANY__";
  }else if(strings){
    return "string";
  }else if(nums){
    return "num";
  }else if(bools){
    return "bool";
  }
  report("getTypeExp didn't know the type", -1);
  return "__ANY__";//error?
}

void splitExp(expression3 * EXP, expression3 * left, expression3 * right, int pos){
  for(int i = 0; i< EXP->bigAtoms->size(); i++){
    if(i < pos){//dont save pos to either
      //add to left exp
      left->bigAtoms->push_back(EXP->bigAtoms->at(i));
    }else{
      //add to right
      right->bigAtoms->push_back(EXP->bigAtoms->at(i));
    }
  }
}

expression3 * parseExpression(const string targetType, const vector<unsigned long> arr, bool allowAssign){//TODO bool allowAssignment?
  debug("parseExpression(3)");
  if(arr.size() != 0 && peek("{")){//parse list if expected
    expression3 * EXP = parseList(arr, targetType);
    debug("parseExpression(3) done");
    return EXP;
  }else if(targetType == "__ANY__" && peek("{")){
    const vector<unsigned long> arr = {0};
    expression3 * EXP = parseList(arr, targetType);
    debug("parseExpression(3) done");
    return EXP;
  }
  report("target Type: "+targetType, -2);
  expression3 * EXP = new expression3();
  bool needOP = false, assignBefore = false, correctSize = arr.size(), doResolve = true, assignable = allowAssign, isConst = false;
  string beingCalledOn = "";
  if(currentClass)
    beingCalledOn = currentClass->name;
  bool isStaticCall = true;
  do{//TODO {} for lists
    bigAtom * bAtom = new bigAtom();
    if(accept("!")||accept("++")||accept("--"))//alternetivly accept(OP) check if frontable
      {bigAtom * nAtom = new bigAtom();nAtom->type = bigAtom::OP; nAtom->op = lastSym->tokenText; EXP->bigAtoms->push_back(nAtom);
      }//TODO negitive anys
    needOP = true;
    bAtom->type =  bigAtom::ATOM;
    bAtom->a = new atom();
    //check if a is static, set static
    //parseAtom(bAtom->a, targetType, beingCalledOn, isStaticCall);
    isStaticCall = parseAtom(bAtom->a, beingCalledOn, isStaticCall);
    beingCalledOn = bAtom->a->helper.type;
    if(bAtom->a->helper.arr.size())
      beingCalledOn = "list";//TODO fix, add support for var type helper everywhere
    //isStaticCall = bAtom->a->baseIsStatic;
    EXP->bigAtoms->push_back(bAtom);
    allowAssign = false;
    while(accept("[")){//replaces a[x] with a.at(x) and [x:y] with sub(x,y,a)
      //this is all put together in one atom(expHolder) so it stays together
      //the atom is then wrapped in a big atom(expAtom) and added to the exp3
      
      bigAtom * expAtom = new bigAtom();expAtom->type = bigAtom::ATOM;
      atom * expHolder = new atom();
      funkCall * FC = new funkCall();
      expHolder->fCall = FC;
      expHolder->type = "funkCall";
      if(peek(":") ||  peek("]")){//add 0
	expHolder->fCall->parameters.push_back(intToExp3(0));
      }else{
	expHolder->fCall->parameters.push_back(parseExpression("num"));//get x
      }
      if(accept(":")){  //sub vector var[start:end]
	if(peek(":") || peek("]")){
	  //expHolder->fCall->parameters.push_back(intToExp3(0));//TODO EXP->bigAtoms->a->toString()+".length()"
	}else{
	  expHolder->fCall->parameters.push_back(parseExpression("num"));//get y
	}
	expression3 * baseValue = atomToExp3(EXP->bigAtoms->back()->a);
	expHolder->fCall->parameters.push_back(baseValue);
	expHolder->fCall->funkName = "sub";
	expHolder->helper = EXP->bigAtoms->back()->a->helper;
	//delete a
	EXP->bigAtoms->pop_back();
	if(accept(":")){//step size
	  expHolder->fCall->parameters.push_back(parseExpression("num"));
	}
	expHolder->helper = bAtom->a->helper.type;//TODO stop gap solution, fix in resolver
      }else{//normal at,  var[itt]
	bigAtom * openAtom = new bigAtom();openAtom->type = bigAtom::OP; openAtom->op = "."; EXP->bigAtoms->push_back(openAtom);//this segment replaces [] with .at()
	expHolder->fCall->funkName = "at";
	
	expHolder->helper = bAtom->a->helper.type;//TODO stop gap solution, fix in resolver
      }
      
      expect("]");
      expAtom->a = expHolder;
      EXP->bigAtoms->push_back(expAtom);
      
      //beingCalledOn shouldnt change from this
      isStaticCall = false;
    }
    allowAssign = assignable;
    isConst = EXP->bigAtoms->back()->a->helper.constant || isConst;
    if(accept("++")||accept("--")){
      if(isConst){
	error("Can not assign value to a constant value", true);
      }
      assignable = false;
      bigAtom * nAtom = new bigAtom();nAtom->type = bigAtom::OP; nAtom->op = lastSym->tokenText; EXP->bigAtoms->push_back(nAtom);
      beingCalledOn = "";
    }else if(peek(".")){
      //do nothing for now. its being called on soemthing so dont change beingCalledOn
    }else{//without a ".", there is no base
      beingCalledOn = "";
    }
    if(isAssignment(peek())){
      if(isConst)
	error("Can not assign value to a constant value", true);
      if(!assignable)
	error("Can not assign value to an expression", true);
      if(assignBefore)
	error("Can only have one assignment per expression", true);
    }
    if(isValidExpOP(allowAssign)){
      bigAtom * nAtom = new bigAtom();
      nAtom->type = bigAtom::OP;
      if(lastSym -> tokenText == "."){//TODO
	assignable = true;//--cant assign if we have other operators besides .
      }else{
	assignable = false;
      }
      if(lastSym -> tokenText == "." && isStaticCall){//replace static calls using the dor with ::
	nAtom->op = "::";//this is because you have to use :: in C++ when calling a static fucntion in a class
      }else
	nAtom->op = lastSym->tokenText;//normally though, just add in the operator
      
      EXP->bigAtoms->push_back(nAtom);
      
      if(isAssignment(lastSym->tokenText)){//if we just assigned something, ie =, +=, etc
	//if(assignBefore)
	//error("Can only have one assignment per expression", true);//unused
	assignBefore = true;//no more assignments allowed
	bigAtom * aAtom = new bigAtom();
	aAtom->exp3 = parseExpression(targetType, arr, false);
	aAtom->type = bigAtom::EXP3;
	doResolve = false;
	EXP->bigAtoms->push_back(aAtom);//TODO currently allows a=b=c
      }else{//if not assignment treat as normal
	needOP = false;
      }
    }
  }while(!needOP);  
  
  /*bool allSame = true;
  for(int i = 0; i<EXP->bigAtoms->size();i++){
    if(EXP->bigAtoms->at(i)->type == bigAtom::ATOM && convertedType(EXP->bigAtoms->at(i)->a->helper.type) != convertedType(targetType)){
      allSame = false;
    }
    }*/
  if(doResolve){
    bigAtom * expAtom = new bigAtom();expAtom->type = bigAtom::ATOM;
    atom * expHolder = new atom();expHolder->type="exp2";
    expHolder->exp2 = resolveExpression2(EXP, targetType);
    expAtom->a = expHolder;
    for(auto i: *(EXP->bigAtoms)){//delete OPs, cant deleate Atoms, they are being used in exp2
      if(i->type == bigAtom::big_type::OP) delete i;
    }
    EXP->bigAtoms->clear();
    EXP->bigAtoms->push_back(expAtom);
  }
  debug("parseExpression(3) done");
  return EXP;
}

bool parseAtom(atom * a, string base, bool baseStatic){
  if(base == "" && currentClass)
    base = currentClass->name;
  debug("parseAtom("+base+")");
  bool baseIsStatic = true;
  if(accept("basic") || accept("twine")){//used to access base functions
    a->type = "lit";
    a->literalValue = new string("");
    baseIsStatic = true;
    a->baseIsStatic = true;
  }else if(accept(TokenData::IDENT)){
    string identName = lastSym->tokenText;
    if(peek("(")){//must be a function call
      Funk *tmp = getFunk(identName, base);
      if(tmp == NULL){
	if(base == "__ANY__"){//this runs the runObjFunk on the any object to try to find function durring runtime
	  a->type = "funkCall";
	  funkCall * FC = new funkCall();
	  a->fCall = FC;
	  //parseFunkCall(a->fCall,identName, base, lastSym->tokenText);//TODO this
	  a->fCall->parameters.push_back(strToExp3(identName));
	  expect("(");
	  while(!accept(")")){//mini parseFunkParams
	    expression3* tmp = parseExpression("");
	    a->helper = tmp->bigAtoms->at(0)->a->exp2->helper;
	    const string typeOf = a->helper.type;
	    if(!(typeOf == "int" || typeOf == "double" || typeOf == "num" || typeOf == "bool")){
	      tmp->ptr = true;
	    }
	    a->fCall->parameters.insert(a->fCall->parameters.begin()+1,tmp);//order gets reversed, do it like this
	    //a->fCall->parameters.push_back(parseExpression("*"));
	    if(!peek(")"))
	      expect(",");
	  }
	  a->helper = var("__ANY__");
	  a->fCall->funkName = "runObjFunk";
	  baseIsStatic = false;
	}else if(base != ""){
          error("No function '"+identName+"' in class '" +base+ "'", false);
        }else
          error("No function '"+identName+"' found", false);
      }else{
	//if(base != "" && baseStatic && !tmp->STATIC)error("Non static call ("+tmp->name+") being called on non-static object ("+lastSym->tokenText, true);
	a->type = "funkCall";
	funkCall * FC = new funkCall();
	a->fCall = FC;
	a->helper = tmp->returnType;
	parseFunkCall(a->fCall,identName, base);//TODO make it so we can pass func(*tmp) so it doenst have to get again?
	baseIsStatic = tmp->STATIC;
      }
    }else if(getClass(identName) != NULL){
      baseIsStatic = true;
      a->baseIsStatic = true;//??
      a->literalValue = new string(identName);
      a->type = "lit";
      a->helper = identName;
    }else if(getVARType(identName) != "DNE" && getClass(getVARType(identName))){
      a->literalValue = new string(identName);
      a->type = "lit";
      a->helper = getVar(identName);
      a->baseIsStatic = false;
      baseIsStatic = false;
    }else{//must be var
      a->type = "lit";
      if(base != ""){//could be member var
        var * varT = getMemberVar(base, identName, baseStatic);//TODO free?
        if(varT != NULL){
          a->literalValue = new string(varT->name);
          a->helper = varT;
          //a->baseIsStatic = varT->STATIC;
          baseIsStatic = varT->STATIC;
          debug("parseAtom() done");
          return varT->STATIC;
        }
      }
      //check in scope vars now
      //string tp = getVARType(lastSym->tokenText);
      var tp = getVar(lastSym->tokenText);
      if(tp.type == "DNE"){//TODO wont work with var a = thing_with_mem_var_a(), a.a
	if(base == "__ANY__"){
	  a->type = "funkCall";
	  funkCall * FC = new funkCall();
	  a->fCall = FC;
	  atom * tmpA = new atom();
	  tmpA->type = "lit";
	  tmpA->literalValue = new string("\""+lastSym->tokenText+"\"");
	  FC->parameters.push_back(atomToExp3(tmpA));
	  
	  FC->funkName = "getObjVar";

	  a->helper = var("__ANY__");
	  baseIsStatic = false;//TODO??-may not be able to tell
	}else if(base != ""){
	  error("Undeclaired var '"+lastSym->tokenText+"' used. Not found in scope or in"+(baseStatic? " static base: ":" base: ")+base, true);
	}else
	  error("Undeclaired var '"+lastSym->tokenText+"' used", true);
        a->helper = var("__ANY__");//what should it defualt to?
      }else{
        a->helper = tp;
      }
       if(base != "__ANY__")
	 a->literalValue = new string(lastSym->tokenText);
      //TODO get if its static
    }
  }else if(acceptType()){//allows thing like className function()
    a->helper = lastSym->tokenText;
    //a->baseIsStatic = true;
    a->type = "lit";
    a->literalValue = new string(lastSym->tokenText);
  }else if(accept("this")){
    if(currentFunk->STATIC)
      error("Cant use 'this' in static function");
    a->helper = lastSym->tokenText;
    //a->baseIsStatic = false;
    baseIsStatic = false;
    a->type = "lit";
    a->literalValue = new string("(*this)");
  }else if(accept("(")){
    a->type = "exp2";
    a->exp2 = parseExpression()->bigAtoms->at(0)->a->exp2;//unwrap it
    a->cast = "";
    a->helper = a->exp2->helper;//TODO -replace with a->exp2->helper;
    expect(")");
  }else if(peek("-") && (sym+1)->tokenType == TokenData::NUM){//not sure about this
    accept("-"); accept(TokenData::NUM);
    a->type = "lit";
    a->helper = var("num");
    a->literalValue = new string(twoSymbAgo->tokenText + lastSym->tokenText);
  }else if(peek("{")){//TODO
    a->type = "exp3";
    a->exp3 = new expression3();
    parseUncertainList(a->exp3);
  }else if(accept("super")){
    baseIsStatic = true;//we say the base is static to format call correctly
    //but call doesnt have to be static, so we dont set "a->baseIsStatic = true"
    if(currentClass == NULL)
      error("There is no 'super' when not in class");
    object * obj = getClass(currentClass->name);
    if(obj == NULL)//should never happen as all classes should extend something, at least object
      error("Can't 'super' when class doesn't extend another class");
    a->literalValue = new string(obj->name);
    a->type = "lit";
  }else{
    //shoudl be hard value: ie 5 or "hello" or false
    if(accept(TokenData::NUM) || accept("true") || accept("false")){
      a->type = "lit";
      a->helper = var("num");//TODO may have to do bool for bool
      a->literalValue = new string(lastSym->tokenText);
    }else if(accept(TokenData::STRING_LITERAL)){
      a->type = "lit";
      a->helper = var("string");
      if(interpMode){
	a->literalValue = new string(lastSym->tokenText.substr(1,lastSym->tokenText.size()-2));//remove quotes
      }else{
	a->literalValue = new string(lastSym->tokenText);
	//a->literalValue = &(lastSym->tokenText);
      }
      a->stringLit = true;
    }else{
      error("Unkown type: "+sym->tokenText +" with type: "+TokenData::getTokenTypeName(sym->tokenType), false);//TODO can cuase infinite loop if true?
      a->type = "__ANY__";
      a->helper = var("__ANY__");
    }
  }
  debug("parseAtom("+a->helper.toString()+") done");
  return baseIsStatic;
}

//defualt pushBackVarScope = true
void parseBlock(Block* b,bool pushBackVarScope){
    debug("parseBlock()");
    expectedIndents++;
    if(pushBackVarScope) pushScope();
    if(accept("{")){
      if(lint) checkBlock((sym-1)->tokenText == "\n" || (sym-2)->tokenText == "\n");
      if(accept("}")){
	if(lint) checkEmpty();
      }else do{
	  /*parseNode * tmp = new parseNode();
	    parseLine(tmp);
	    b->Lines.push_back(*tmp);*/
	  parseNode * tmp = new parseNode();
	  parseLine(tmp);
	  b->Lines.push_back(tmp);
	}while(!accept("}"));
    }else{
      if(lint) checkBlock((sym-1)->tokenText == "\n" || (sym-2)->tokenText == "\n");
      parseNode * tmp = new parseNode();
      parseLine(tmp);
      b->Lines.push_back(tmp);
    }
    //Scope stuff
    int numOfPulls = pullScope();
    if(numOfPulls){
      parseNode * tmp = new parseNode();
      tmp->type = "C++";//Not really C++ but easy to print data
      tmp->data = "";
      for(;numOfPulls;numOfPulls--){
	tmp->data+="}";
      }
      b->Lines.push_back(tmp);
    }//END scope stuff
    expectedIndents--;
    //TODO expect new line if not else coming up
    debug("parseBlock() done");
  }
  void parseLine(parseNode* PN){
    debug("parseLine()");
    currentLine.clear();
    totalLines++;
    //TODO better way of checking for class types?
    if((sym+1)->tokenText != "." && acceptType()){//TODO i feel like this is not the best way to do this
      PN->type = "varAss";
      PN->theThing = new varAssign();
      parseVarAssign((varAssign*)PN->theThing, false, lastSym -> tokenText);
      //}else if(accept(TokenData::IDENT)){
    }else if(peek(TokenData::IDENT) && (sym+1)->tokenText != "."){
      accept(TokenData::IDENT);
      parseIdent(PN);
    }else if(peek() == "this"){
      PN->type = "exp3";
      PN->theThing = parseExpression("*");
    }else if(accept(TokenData::RESERVED)){
      parseKW(PN);
    }else if(accept("{") || accept("}") || accept("(") || accept(")")){
      error("Miss matched "+lastSym->tokenText+" ", false);
    }else if(peek("class")){
      if(currentClass){
	error("Class being declaired inside other class ("+currentClass->name+")");
      }else{
	error("Class declaired in wrong spot");
      }
    }else if(accept(TokenData::FILE_CHANGE)){
      currentParsingFile = lastSym->tokenText;
      
    }else if(accept("import")){//ignore these here
      expect(TokenData::IMPORT);//currently parses all imported files in preScan()
      PN->type = "IMPORT";
      PN->data = lastSym->tokenText;
    }else{
      //try to parse exp
      PN->type = "exp3";
      PN->theThing = parseExpression();
    }
    if(lint) checkLine(currentLine);
    debug("parseLine() done");
  }
//Parsees lines that begin with reserved Key Word
void parseKW(parseNode* PN){
    debug("parseKW()");
    if(lastSym -> tokenText == "if"){
      PN->type = "IF";
      IF * tmp =  new IF();
      PN->theThing = tmp;
      parseIF(tmp);
    }else if(lastSym -> tokenText == "for"){
      PN->type = "FOR2";
      FOR2 * tmp =  new FOR2();
      PN->theThing = tmp;
      parseFOR(tmp);
    }else if(lastSym -> tokenText == "while"){
      PN->type = "WHILE";
      WHILE * tmp = new WHILE();
      PN->theThing = tmp;
      parseWHILE(tmp);
    }else if(lastSym -> tokenText == "do"){
      PN->type = "WHILE";
      PN->theThing = parseDoWHILE();
    }else if(lastSym -> tokenText == "return"){
      PN->type = "ret";
      if(currentFunk->returnType.type == "void"){
	if(!FORCE){
	  error("Return in void function", true);//TODO allow empty return
	}else{
	  delete parseExpression(currentFunk->returnType);//parse it, then throw it away
	  PN->type = "SINGLE_COM"; PN->data = "FORCE-removed invalid return\n";
	}
      }else{
	PN->theThing = parseExpression(currentFunk->returnType);
      }
      /*}else if(lastSym -> tokenText == "int"||lastSym -> tokenText == "double"||lastSym -> tokenText == "string"||lastSym -> tokenText == "bool"||lastSym -> tokenText == "__ANY__"){
      PN->type = "varAss";
      varAssign * tmp = new varAssign();
      PN->theThing = tmp;
      parseVarAssign(tmp, false, lastSym -> tokenText);
      tmp->newVar = true;*/
    }else if(lastSym->tokenText == "const"){
      varAssign * va = new varAssign();
      if(acceptType()){
	parseVarAssign(va, true, lastSym->tokenText);
      }else{
	parseVarAssign(va, true, "__ANY__");
      }
      va->newVar = true;
      PN->type = "varAss";
      PN->theThing = va;
    }else if(lastSym->tokenText == "try"){
      PN->type = "TRY";
      tryCatch * tc = new tryCatch();
      parseBlock(tc->tryBlock);
      expect("catch");
      bool needClosing = false;
      if(accept("(")) needClosing = true;
      //TODO get exception type?
      expect(TokenData::IDENT);
      tc->exceptionName = lastSym->tokenText;
      setVar(lastSym->tokenText);//string?
      if(needClosing) expect(")");
      parseBlock(tc->catchBlock);
      removeVar(tc->exceptionName);
      PN->theThing = tc;
    }else if(lastSym->tokenText == "throw"){
      PN->type = "throw";
      //PN->theThing = parseExpression("string");
      throw_e * e = new throw_e();
      parseThrow(e);
      PN->theThing = e;
    }else if(lastSym -> tokenText == "Cpp_code"){
      PN->type = "C++";
      expect(TokenData::STRING_LITERAL);
      PN->data = (lastSym->tokenText).substr(1, (lastSym->tokenText).size() - 2);;
    }else if(lastSym -> tokenText == "class"){
	error("Invalid placement of 'class' keyword");
    }else if(lastSym->tokenText == "continue"){
      PN->type = "C++";
      PN->data = "continue;";
    }else if(lastSym->tokenText == "static"){
      error("Use of 'static' outside of class");
    }else{
      error("Invalid use of key word (it might just be a word reserved by C++)");
    }
    debug("parseKW() done");
  }

void parseThrow(throw_e * e){
  debug("parseThrow()");
  e->file = currentParsingFile;
  e->line = lastSym->line;
  e->func = currentFunk->name;
  e->what = parseExpression(string("string"));
  debug("parseThrow() done");
}

void parseIF(IF* IF){
  debug("parseIF()");
  currentFunk->complexity.conditionals+=1;
  IF->exps3->push_back(parseExpression(string("bool")));//parses expression and adds it to IF object
  Block * tmp = new Block;
  IF->blockStatments->push_back(tmp);
  parseBlock(tmp);
  while(accept("elif")){
    currentFunk->complexity.conditionals+=1;
    IF->exps3->push_back(parseExpression(string("bool")));
    Block * tmp = new Block();
    IF->blockStatments->push_back(tmp);
    parseBlock(tmp);
  }
  if(accept("else")){
    Block * tmp = new Block();
    IF->blockStatments->push_back(tmp);
    parseBlock(tmp);
  }
  debug("parseIF() done");
}

void parseWHILE(WHILE* WHILE){
  debug("ParseWHILE()");
  currentFunk->complexity.loops+=1;
  WHILE->exp3 = parseExpression(string("bool"));
  parseBlock(WHILE->blockState);//whiles block is inititated when WHILE is init
  debug("parseWHILE() done");
}

WHILE* parseDoWHILE(){
  debug("ParseDoWHILE()");
  currentFunk->complexity.loops+=1;
  WHILE* newWhile = new WHILE(false);
  parseBlock(newWhile->blockState);
  expect("while");
  newWhile->exp3 = parseExpression(string("bool"));
  debug("parseDoWHILE() done");
  return newWhile;
}
  bool checkIn(){
    if(peek("in"))
      return true;
    vector<TokenData>::iterator tmp0 = sym;
    vector<TokenData>::iterator tmp1 = lastSym;
    vector<TokenData>::iterator tmp2 = twoSymbAgo;
    nextSym();
    if(accept("in")){
      sym = tmp0;
      lastSym = tmp1;
      twoSymbAgo = tmp2;
      return true;
    }
    nextSym();
    if(accept("in")){
      sym = tmp0;
      lastSym = tmp1;
      twoSymbAgo = tmp2;
      return true;
    }
    sym = tmp0;
    lastSym = tmp1;
    twoSymbAgo = tmp2;
    return false;
  }
  void parseFOR(FOR2 * F){
    debug("parseFOR(2)");
    currentFunk->complexity.loops+=1;
    pushScope();
    if(checkIn()){
      F->for_in = true;//set type
      parseForIn(F);
    }else{
      F->for_in = false;//set type
      parseNormalFOR(F);
    }
    Block * blk = new Block();
    F->blockExpression = blk;
    parseBlock(blk, true);
    pullScope();
    debug("parseFOR(2) done");
  }
  void parseForIn(FOR2 * F){//TODO, needs work
    debug("parseForIn()");
    currentFunk->complexity.loops+=1;
    var * me = new var();
    if(accept(TokenData::RESERVED)){
      me->type = lastSym->tokenText;
    }else
      me->type = "__ANY__";
    expect(TokenData::IDENT);
    me->name = lastSym->tokenText;
    F->ittName = me;
    setVar(me->name,me->type);//not sure if right type
    expect("in");//should be there
    expression3 * a = parseExpression("*");//TODO fix this mess
    if(accept(":")){
      F->start3 = a;//now a should be a num
      a->cast = "num";
      F->end3 = parseExpression("num");
    }else{
      F->end3 = a;
    }
    //if(peek(":")){    }//TODO step size
    debug("parseForIn() done");
  }
void parseNormalFOR(FOR2 * F){//TODO this is broken
  debug("parseNormalFOR()");
  currentFunk->complexity.loops+=1;
  parseNode * me = new parseNode();
  F->lineOne = me;
  parseLine(F->lineOne);
  expect(";");//is this what i want to put between statments?
  F->lineTwo3 = parseExpression("num");//or bool?
  expect(";");
  parseNode * me3 = new parseNode();
  F->lineThree = me3;
  parseLine(F->lineThree);
  debug("parseNormalFOR() done");
}

/*
parses idents, could either be function call or var assignment
*/

void parseIdent(parseNode * pn){//TODO add support for x++
  debug("parseIdent()");
  string varName = lastSym->tokenText;
  bool needToSetVar = false;
  if(peek("(") && isCallable(lastSym->tokenText)){//&&expect("(")){//if you want to allow vars with same name as functions, change this
    pn->type = "funkCall";
    funkCall * me = new funkCall();
    pn->theThing = me;
    parseFunkCall(me, varName);
  }else if(accept("=")||accept("+=")||accept("-=")||accept("*=")||accept("/=")||accept("%=")||accept("^=")){//TODO make this use parseVarAssign
    if(isConstant(varName) && isLocal(varName))//TODO add warning about shadow var(vars with same name in diffrent scope)
      error("Trying to set constant varriable '"+varName+"' to a new value", true);
    pn->type = "varAss";
    varAssign * me = new varAssign();
    me->OP = lastSym->tokenText;
    pn->theThing = me;
    var * tVar = new var;
    tVar->name = varName;
    if(getVARType(varName) == "DNE" || isConstant(varName)){//if it is constant, that means a new var must be made to shadow the constant var
      if(isConstant(varName))
	warn("Varriable '"+varName+"' is shadowing existing constant varriable with same name");
      //setVar(varName);
      me->newVar = true;
      needToSetVar = true;
      tVar->type = "__ANY__";
      if(lint){
	checkTypeAllow(varName);
	checkVarName(varName);
      }
      me->exp3 = parseExpression("__ANY__", {}, false);
    }else{
      tVar->type = "";
      me->exp3 = parseExpression(getVar(varName), false);

      }
     me->VAR = tVar;
     /*if(currentFunk->name == "__userMain__" && currentClass == NULL && tVar->type != "" && scopeDepth == 1){
      tVar->startingValue = me->exp3;
      Prog->globalVars.push_back(tVar);
      pn->type = "EMPTY";
      }*/
    //}else if(accept("++") || accept("--")){
    /*if(getVARtype(varName) != "DNE"){
      pn->type = "C++";
      pn->data = twoSymbAgo->tokenText + lastSym->tokenText;
    }else{
      
    }*/
    /*}else if(peek("[")){
    twoSymbAgo = lastSym;
    sym = lastSym;
    pn->type = "exp3";
    //pn->theThing = parseExpression(getVARType(sym->tokenText));
    pn->theThing = parseExpression();*/
  }else{
    /*if(peek() == "("){
      error("Unknown function "+lastSym->tokenText);
    }else{//when only a var is provided in interpreter mode, this should print it, not throw error
      error("Unkown identity "+lastSym->tokenText);
      }*/
    twoSymbAgo = lastSym;
    sym = lastSym;
    pn->type = "exp3";
    pn->theThing = parseExpression();
  }
  if(needToSetVar)
    setVar(varName);
  debug("parseIdent() done");
}
var * parseClassVar(){
  debug("parseClassVar()");
  var * a = new var(parseVar());
  /*if(accept("const"))
    a->constant = true;
  if(accept("static")){
    a->STATIC = true;
    //a->atomic = true;//TODO
  }
  bool typeWasSet;
  if(acceptType()){
    typeWasSet = true;
    a->type = lastSym->tokenText;
  }else{
    typeWasSet = false;
    a->type = "__ANY__";
  }
  expect(TokenData::IDENT);
  a->name = lastSym->tokenText;
  if(isLocal(a->name))//TODO--this check doesnt currently work
    error("Redefinition of class var: "+a->name, true);//TODO
  if(lint && currentLineOn){
    checkVarName(a->name, a->constant);
    checkTypeAllow(a->name, !typeWasSet);
    }*/
  if(accept("="))
    a->startingValue = parseExpression(a);
  debug("parseClassVar() done");
  return a;
}

// (TYPE)([#]..) NAME = VALUE
void parseVarAssign(varAssign * va, bool constant, const string varType){
  debug("parseVarAssign("+varType+")");
  va->newVar = true;
  bool isList = false;
  var * tVar;
  tVar = new var();
  tVar->constant = constant;
  tVar->type = varType;
  bool isGlobe = false;
  if(currentFunk->name == "__userMain__")
    isGlobe = true;
  
  while(accept("[")){
    if(accept(TokenData::NUM)){//defined size
      long listSize= stoi(lastSym->tokenText);//int, long, unsigned?
      if(listSize <= 0)
        error("List size must be >0");
      tVar->arr.push_back(listSize);
    }else{//undefined size
      tVar->arr.push_back(0);
    }
    expect("]");
    isList = true;
    tVar->isList = true;
  }

  expect(TokenData::IDENT);//get name
  const string varName = lastSym->tokenText;
  tVar->name = varName;

  expect("=");//get OP

  va->OP = lastSym->tokenText;

  if(getVARType(varName) != "DNE"){//check unique and lint
    warn("Varriable '"+varName+"' with type '"+varType+"' overides higher-scoped var with same name and type '"+getVar(varName).toString()+"'");
  }else if(lint){
    checkVarName(varName, constant);
  }

  /*if(isList){//parse starting value
    va->exp3 = parseList(tVar->arr, varType);
  }else{
    va->exp3 = parseExpression(tVar->type);
    }*/

  if(accept("NULL")){
    va->exp3 = NULL;
  }else{
    va->exp3 = parseExpression(*tVar, false);
  }

  //add var to scope
  //setVar(varName,varType,false,constant);
  setVar(tVar, isGlobe);
  va->VAR = tVar;
  debug("parseVarAssign() done");
}

var * getMemberVar(const string& className, const string& varName, bool STATIC_BASE){
  object * tmp = getClass(className);
  if(tmp == NULL){
    report("Looking for var: "+varName+" in class: "+className+" but class doesnt exist; getMemberVar",-2);
    return NULL;
  }
  for(int i = 0; i<tmp->memberVars.size(); i++){
    if(varName == tmp->memberVars.at(i)->name){
      if(STATIC_BASE != tmp->memberVars.at(i)->STATIC){//only give dynamic vars when not static
	error("this  thing");
	return NULL;//give error?
      }
      return tmp->memberVars.at(i);
    }
  }
  if(tmp->extends != "")
    return getMemberVar(tmp->extends, varName, STATIC_BASE);
  report("Looking for var: "+varName+" in class: "+className+" but var wasn't found; getMemberVar",-2);
  return NULL;
}

Funk * functionsToFunk(functions func){//TODO leaks mem unless ptr is saved
  report("functionsToFunk: "+func.name+"--", -2);
  Funk * tmp = new Funk();
  tmp->name = func.name;
  tmp->returnType = var(func.returnType);//doesnt allow []
  if(func.params == "void")
    return tmp;
  vector<string> params = split(func.params,",");
  for(int x = 0;x<params.size();x++){
    vector<string> singleParam = split(params.at(x), " ");
    var newVar = var();//TODO fix to make this cleaner, not new/delete
    newVar.type = singleParam.at(0);
    if(split(singleParam.at(1),"=").size() == 2){
      newVar.name = split(singleParam.at(1), "=").at(0);
      newVar.startingValue = stringToExp(split(singleParam.at(1),"=").at(1));
    }else if(split(singleParam.at(1),baseNameSym).size() == 2){
      newVar.name = split(singleParam.at(1), "=").at(0);
      newVar.startingValue = stringToExp(split(singleParam.at(1),"=").at(1));
    }else{
      newVar.name = singleParam.at(1);
    }
    tmp->parameters.emplace_back(newVar);
  }
  if(func.alias.length())
    tmp->alias = func.alias;
  return tmp;
}

object * nameSpaceToObject(nameSpace n){
  object * obj = new object;
  obj->name = n.name;
  for(int j = 0;j<n.dynamicMethods.size();j++){
    //TODO
    obj->memberFunks.push_back(functionsToFunk(n.dynamicMethods[j]));
  }
  for(int j = 0;j<n.staticMethods.size();j++){
    //TODO
    Funk * me = functionsToFunk(n.staticMethods[j]);
    me->STATIC = true;
    obj->memberFunks.push_back(me);
  }
  for(int i = 0; i<n.classConstants.size(); i++){
    var * tmp = new var();
    tmp->name = n.classConstants.at(i).name;
    tmp->type = n.classConstants.at(i).type;
    tmp->constant = true;
    //tmp->STATIC = true;
    obj->memberVars.push_back(tmp);
  }
  for(int i = 0; i<n.classStaticVars.size(); i++){
    var * tmp = new var();
    tmp->name = n.classStaticVars.at(i).name;
    tmp->type = n.classStaticVars.at(i).type;
    tmp->STATIC = true;
    obj->memberVars.push_back(tmp);
  }
  for(int i = 0; i<n.classDynamicVars.size(); i++){
    var * tmp = new var();
    tmp->name = n.classDynamicVars.at(i).name;
    tmp->type = n.classDynamicVars.at(i).type;
    obj->memberVars.push_back(tmp);
  }
  return obj;
}

object * getClass(string name){//TODO
  debug("getClass(): "+name+"--");
  for(int i = 0;i<Prog->classes.size();i++){
    if(name == Prog->classes.at(i)->name){
      return Prog->classes.at(i);
    }
  }
  for(int i = 0; i<TwineObjects.size(); i++){
    if(TwineObjects.at(i)->name == name)
      return TwineObjects.at(i);
  }
  for(int i = 0; i<sizeof(nameSpaces)/sizeof(nameSpaces[0]);i++){
    if(name == nameSpaces[i].name){
      object * obj = nameSpaceToObject(nameSpaces[i]);
      TwineObjects.push_back(obj);
      return obj;
    }
  }
  debug("getClass() no class: "+name+" found-- ");
  return NULL;
}
branch * getBranchType(string name){//TODO
  if(name == "this")
    if(currentClass){
      name = currentClass->name;
    }else{error("Refrence to 'this' unavalable, not used inside class or branch");}//TODO
  for(int i = 0;i<Prog->branches.size();i++){
    if(name == Prog->branches.at(i)->name){
      return Prog->branches.at(i);
    }
  }
  error("Get branch return void. No branch found with name: "+name);
  return NULL;
}
Funk * applyTemplate(Funk * f, object * obj){//TODO
  debug("applyTemplate--");
  report("applyTemplate with funk: "+f->name+" in obj: "+obj->name, -2);
  /*if(obj->isBranch || obj->name == "channel" || obj->name == "runnable"){
    if(f->name == "write"){
      (f->parameters.at(0).type = ((branch *) obj)->type);
      cout<<"BRANCH WRITE: "<<f->parameters.at(0).type<<endl;
      exit(99);
      return f;
    }
    if(f->name == "read"){
      (f->returnType = ((branch *) obj)->type);
      return f;
    }
    }*/
  //TODO more
  //build vector of template names
  /*
  vector<templateType *> temps = vector<templateType *>();//this is so it can take temps from an obj+extends
  for(unsigned i = 0; i<obj->templates.size(); i++){
    temps.push_back(obj->templates.at(i));
  }
  for(unsigned i=0; i<temps.size(); i++){
    if(f->returnType == temps.at(i)->name)
      f->returnType = temps.at(i)->type;
    for(unsigned j=0; j<f->parameters.size();j++){
      if(f->parameters.at(j) == temps.at(i)->name)
      f->parameters.at(j).type = temps.at(i)->type;
    }
  }
  */
  return f;
}

//TODO this is called too often, can be optimnised a lot
Funk * getFunk(string name, string Class){
  debug("getFunk()--");
  report("Function: "+name+" Class: "+Class, -2);

  if(Class != "" && (Class != "twine" || Class != "basic")){
    object* obj = getClass(Class);
    if(obj == NULL){//not sure about this
      obj = getBranchType(Class);
    }
    if(obj == NULL){
      error("No class or branch '"+Class+"' found");
      return NULL;
    }else{
      if(name == "receive" && obj->isBranch){//TODO change if/when __ANY__s can hold branches
	if(((branch *) obj)->read == NULL){
	  ((branch *) obj)->read = new Funk;
	  ((branch *) obj)->read->name = "receive";
	  ((branch *) obj)->read->returnType = var(((branch *) obj)->type);
	}
	return ((branch *) obj)->read;
      }
      if(name == "send" && obj->isBranch){
	if(((branch *) obj)->write == NULL){
	  ((branch *) obj)->write = new Funk;
	  ((branch *) obj)->write->name = "send";
	  ((branch *) obj)->write->returnType = var("void");
	  ((branch *) obj)->write->parameters.push_back(var("data", ((branch *) obj)->type));
	}
	return ((branch *) obj)->write;
      }
    }
    for(int i = 0; i<obj->memberFunks.size(); i++){//check member functions
      if(obj->memberFunks.at(i)->name == name){
        return applyTemplate(obj->memberFunks.at(i), obj);
      }
    }
    //if no member functions found, check extended classes
    if(obj->extends != "")
      return getFunk(name, obj->extends);
  }
  //check for global functions
  for(int i = 0;i<TwineFunks.size(); i++){//checks if built in function previusly called
    if(TwineFunks.at(i)->name == name){
      return TwineFunks.at(i);
    }
  }
  for(int i = 0;i<sizeof(builtInFunctions)/sizeof(*builtInFunctions);i++){
    if(builtInFunctions[i].name == name){
      Funk * tmp = functionsToFunk(builtInFunctions[i]);
      TwineFunks.push_back(tmp);
      return tmp;
    }
  }
  //check user functions
  for(int i = 0;i<Prog->functions.size(); i++){
    if(Prog->functions.at(i)->name == name){
      return Prog->functions.at(i);
    }
  }
  //check constructors
  for(int i = 0;i<Prog->classes.size(); i++){
    if(Prog->classes.at(i)->constructors.size() && Prog->classes.at(i)->name == name)
      return Prog->classes.at(i)->constructors.at(0);
  }
  //same thing for interp functions
  if(interpMode)
    for(int i = 0;i<sizeof(interpFunctions)/sizeof(interpFunctions[0]);i++){
      if(interpFunctions[i].name == name){
        Funk * tmp = functionsToFunk(interpFunctions[i]);
        TwineFunks.push_back(tmp);
        return tmp;
      }
    }
  //error or warn
  report("No function found in getFunk; name: "+name+" class was: "+Class, -2);
  return NULL;
}

bool removeFunk(string name, string Class){
  if(Class != ""){
    report("Not implimented 909", 4);
    throw 909;
  }
  for(int i = 0; i<Prog->functions.size(); i++){
    if(Prog->functions.at(i)->name == name){
      delete Prog->functions.at(i);
      Prog->functions.erase(Prog->functions.begin()+i);
      return true;
    }
  }
  return false;
}
bool removeClass(string name){//TODO
  for(int i = 0; i<Prog->classes.size(); i++){
    if(Prog->classes[i]->name == name){
      delete Prog->classes.at(i);
      Prog->classes.erase(Prog->classes.begin()+i);
      return true;
    }
  }
  return false;
}
void parseFunkCallParameters(funkCall * fc, Funk * F){
  debug("parseFunkCallParameters()");
  expect("(");
  bool isVer = false;

  //initialize all parameters to NULL
  for(int i = 0; i < F->parameters.size(); i++){
    if(F->parameters.at(i).secondaryType == "...")//TODO????
      isVer = true;
    fc->parameters.push_back(NULL);//abc123
  }
  bool needCamma = false, exitLoop = false;
  int currentParam = 0;

  //set all vars given, with name=vale and by value, value
  while(!accept(")") && !exitLoop){//TODO loop through function expected params
    if(needCamma)
      expect(",");
    needCamma = true;
    if(peek(TokenData::IDENT) && (sym+1)->tokenText == "="){//ident = value
      accept(TokenData::IDENT);
      string lookingFor = lastSym->tokenText;
      accept("=");//already know its there
      bool found = false;
      for(int i = 0; i < F->parameters.size() && !found; i++){
        if(F->parameters.at(i).name == lookingFor){
          found = true;
          expression3 * tmp = fc->parameters.at(i);
          fc->parameters.at(i) = parseExpression(F->parameters.at(i).type);
          //if(tmp != NULL) delete tmp;//get rid of possible previous value
        }else{
	  //set defualt value
	  if(fc->parameters.at(i) == NULL)
	    fc->parameters.at(i) = F->parameters.at(i).startingValue;
	}
      }
      if(!found) error("Unknown paramter '"+lookingFor+"', in call to function '"+fc->funkName+"'");
      /*}else if(F->parameters.at(currentParam).secondaryType == "..."){ //variadic params
	vector<expression3 *> dynamicParams;
	while(!peek(")")){//not end of func call
	  dynamicParams.push_back(parseExpression(F->parameters.at(currentParam).type));
	}//end of while
	if(dynamicParams.size()){
	  //fc->parameters.push_back(intToExp3(dynamicParams.size()));//add # of dynamic params
	  for(int i = 0; i < F->parameters.size(); i++){if(F->parameters.at(i).name == "argLen"){
	      fc->parameters.at(i) = intToExp3(dynamicParams.size());
	    }}//set number of variable params
	  for(int dynamicParamPos = 0; dynamicParamPos<dynamicParams.size(); dynamicParamPos++){
	    fc->parameters.push_back(dynamicParams.at(dynamicParamPos));
	  }
	  }  */    
    }else{//parse regular parameter: value, value
      if(currentParam >= F->parameters.size()){//veradic function-TODO check
	if(F->dynamicParamType != ""){
	  int dSize = 0;
	  int pSize = fc->parameters.size();
	  fc->parameters.push_back(NULL);
	  while(!peek(")")){//not end of func call
	    dSize++;
	    expression3 * tmp = parseExpression(F->dynamicParamType);
	    if(F->dynamicParamType != "__ANY__")
	      tmp->cast = "__ANY__";
	    fc->parameters.push_back(tmp);
	    if(!peek(")")) expect(",");
	    currentParam++;
	  }
	  fc->parameters.at(pSize) = intToExp3(dSize);
	}else{
	  error("Too many parameters passed to function '"+F->name+"', ", + F->parameters.size()+" parameters in function");
	  parseExpression();
	  exitLoop = true;//TODO just parse last of params so we can look at rest of file
	}
      }else{//normal param parsing
	//replaces existing parameter
	expression3 * tmp = fc->parameters.at(currentParam);
	fc->parameters.at(currentParam) = parseExpression(F->parameters.at(currentParam).type);
	if(tmp != NULL) delete tmp;
	currentParam++;
      }
    }
  }//end while loop

  //remove all un-used parameters and check for required parameters
  //currently all trailing nulls that have a defualt parameter can be saftly removed entierly, unless were in interpreting mode
  bool canRemove = true;
  for(int i = F->parameters.size()-1; i > -1; i--){
    if(fc->parameters.at(i) == NULL){
      if(F->parameters.at(i).startingValue == NULL && F->parameters.at(i).secondaryType != "..."){
        error("Missing parameter '"+F->parameters.at(i).name+"' (" + F->parameters.at(i).type+") to function call: "+F->name);
      }else if(interpMode && F->parameters.at(i).secondaryType != "..."){//save all defualt paramas
        fc->parameters.at(i) = F->parameters.at(i).startingValue;
      }else if(canRemove){//get rid of defualt params
	fc->parameters.pop_back();
      }
    }else{//not null
      canRemove = false;
    }
  }
  //if(fc->isVer &&
  if(F->dynamicParamType != "" && F->parameters.size() == currentParam)
    fc->parameters.push_back(intToExp3(0));

  //set alias to real name
  if(F->alias != ""){
    fc->funkName = F->alias;
  }
  debug("parseFunkCallParameters() done");
}

void parseFunkCall(funkCall * fc, string name, string base){
  debug("parseFunkCall()");
  if(name == ""){
    expect(TokenData::IDENT);
    fc->funkName = lastSym->tokenText;
  }else{
    fc->funkName = name;
  }
  dummy();
  //should match function name to all known functions
  //then check for # and type of params
  //need to look ahead to check for type/if call is ambiguess
  if(base == "" && currentClass)
    base = currentClass->name;
  Funk * funkPtr = getFunk(name, base);
  
  if(funkPtr != NULL){
    parseFunkCallParameters(fc, funkPtr);
  }else{
    error("Function not found: "+name);
    //report("Function not found: "+name,-1);
    expect("(");
    bool needCamma = false;
    while(!accept(")")){//TODO need this?
      if(needCamma) expect(",");
      needCamma = true;
      fc->parameters.push_back(parseExpression("__ANY__"));
    }
  }
  debug("parseFunkCall() done");
}


bool isCallable(string name){//TODO concolidate functions
  debug("isCallable ? ");
  for(int i = 0;i<Prog->functions.size(); i++){
    if(Prog->functions.at(i)->name == name){
      return true;
    }
  }
  for(int i = 0;i<TwineFunks.size(); i++){
    if(TwineFunks.at(i)->name == name){
      return true;
    }
  }
  for(int i = 0;i<sizeof(builtInFunctions)/sizeof(*builtInFunctions);i++){
    if(builtInFunctions[i].name == name){
      return true;
    }
  }
  for(int i = 0;i<Prog->classes.size(); i++){//TODO more effecent way?
    if(getFunk(name, Prog->classes.at(i)->name) != NULL)//def better way to do this
      return true;
  }
  //interp functions
  //TODO should i just have one list of all functions, with bool isInterp ?
  for(int i = 0;i<sizeof(interpFunctions)/sizeof(*interpFunctions);i++){
    if(interpFunctions[i].name == name){
      return true;
    }
  }
  return false;
}

  //------------HELPERS------------//
bool expect(TokenData::Type s,bool allowForce){
  if(accept(s))
    return true;
  error("Expected: "+TokenData::getTokenTypeName(s)+" but instead found: "+sym->tokenText+" which is type: "+TokenData::getTokenTypeName(sym->tokenType)+'\n');
  return false;
}
bool expect(string s,bool allowForce){
  if(accept(s))
    return true;
  if(FORCE && allowForce){
    forceAdd(s);
    return true;
  }
  error("Expected: "+s+" but instead found: "+sym->tokenText+'\n');
  return false;
}

bool accept(TokenData::Type s){
  if(sym->tokenType == s){
    debug("Eating type "+TokenData::getTokenTypeName(sym->tokenType)+" ");
    nextSym();
    return true;
  }
  return false;
}
bool accept(string s){
  if(sym->tokenText == s){
    debug("Eating ");
    nextSym();
    return true;
  }
  return false;
}
bool shouldThread(const Funk* i_am_a_funkky_pointer){
  if(i_am_a_funkky_pointer->open || (i_am_a_funkky_pointer->totalComplexity() >= threadComplexityThreshold &&
				     i_am_a_funkky_pointer->totalComplexity() >= minthreadComplexityThreshold && !i_am_a_funkky_pointer->closed)){
    return true;
  }  
  return false;
}
bool peek(TokenData::Type s){
  if(sym->tokenType == s){
    return true;
  }
  return false;
}
bool peek(string s){
  if(sym->tokenText == s){
    return true;
  }
  return false;
}
string peek(){
  return sym->tokenText;
}
//0;136;0c
//^[[>0;136;0c
void nextSym(){
  twoSymbAgo = lastSym;
  lastSym = sym;
  
  if(currentLineOn)currentLine.push_back(*sym);

  do{
    sym++;

    if(sym->tokenType == TokenData::BLOCK_COMMENT){
      totalComments++;
      if(currentLineOn){
	parseNode * com = new parseNode();
	com->type = "BLOCK_COM";
	com->data = sym->tokenText;
	currentFunk->funkBlock->Lines.push_back(com); concecutiveNewLines = 0;
      }
    }else if(sym->tokenType == TokenData::LINE_COMMENT){
      totalComments++;
      if(currentLineOn){
      parseNode * com = new parseNode();
      com->type = "SINGLE_COM";
      com->data = sym->tokenText;
      currentFunk->funkBlock->Lines.push_back(com); concecutiveNewLines = 0;}
    }else if(sym->tokenType == TokenData::LINE_END && lint){//TODO should i check for lint and lint flag? will new line be here for any other reason?
      concecutiveNewLines++;
      if(getLintFlag("EXCESS_NEW_LINE") && getLintFlag("EXCESS_NEW_LINE") < concecutiveNewLines && currentLineOn)
	lintReport("Too many blank lines in a row ("+to_string((long long) concecutiveNewLines)+"/"+to_string((long long) getLintFlag("EXCESS_NEW_LINE"))+")", 1);
    }else{
      concecutiveNewLines = 0;
    }
  }while(sym->tokenType == TokenData::BLOCK_COMMENT || sym->tokenType == TokenData::LINE_COMMENT || //sym->tokenType == TokenData::FILE_CHANGE ||
	 (sym->tokenType == TokenData::WHITESPACE || sym->tokenType == TokenData::LINE_END));
}

void forceAdd(string newSym){//TODO
  dummy();//fix up etc
  report("Force adding sym: "+newSym,0);
  TokenData * newToken = new TokenData(newSym,sym->line,sym->charPos,TokenData::FORCE_ADDED);//should change type
  //add token to token list
  if(sym == tokenList->end()){
    tokenList->push_back(*newToken);
  }else{
    tokenList->insert(sym,*newToken);
  }
  if(currentLineOn)
    currentLine.push_back(*newToken);
  //point sym to new token
}
void forceSkip(){
  report("Force skipping: "+sym->tokenText,0);
  nextSym();
}

void error(const string& msg, bool passable){
  //currentLine.push_back(*sym);
  string line = "";//sometimes thie repeats the last word twice
  for(int i = 0; i<currentLine.size();i++){//TODO add color
    line+=currentLine.at(i).tokenText;//+" ";//TODO only add " " if not saving white spaces
    if(!SAVE_WHITESPACE)
      line+=' ';
  }
  errors++;
  string errorMsg;
  if(string(getenv("TERM")) == string("xterm")){
    errorMsg = "\033[31mERROR\033[0m";//red error message
  }else{
    errorMsg = "ERROR";
  }
  if(!(currentFunk->name == "__userMain__" && currentClass != NULL))//if were in a class but not a function, dont say were in function '__userMain__'
    errorMsg += " in function "+currentFunk->name;
  if(currentClass != NULL)
    errorMsg += " in class "+currentClass->name;
  errorMsg += "\n";
  
  if(!interpMode)
    errorMsg += "'"+ currentParsingFile+"' ";
    
  errorMsg += to_string((unsigned) sym->line)+":"+to_string((unsigned) sym->charPos)+": "+msg+'\n';
  errorMsg += line + '\n';
  if(!passable){
    throw invalid_argument(errorMsg);
  }else{
    report(errorMsg, 4);
  }
  if(getFlag("EXIT_ON_ERROR"))//should just use a dict for these flags
    exit(1);
}
void warn(const string& msg){//TODO make look a bit nicer
  if(getFlag("IGNORE_ALL_WARNING")){
    return;
  }
  if(getFlag("WARNINGS_ARE_ERRORS")){
    error(msg);
    return;
  }
  string warningMsg;
  if(string(getenv("TERM")) == string("xterm")){
    warningMsg = "\033[35mWARNING\033[0m";//purple warning msg
  }else{
    warningMsg = "WARNING";
  }
  if(currentFunk != NULL)
    warningMsg += " in function "+currentFunk->name;
  if(currentClass != NULL)
    warningMsg += " in class "+currentClass->name;
  warningMsg += "\n";
  if(!interpMode)
    warningMsg += "'" + currentParsingFile +"' ";

  //use lastSym to avoid reporting next line when being called from resolver
  warningMsg += to_string((long long) lastSym->line)+":"+to_string((long long) sym->charPos)+": "+msg + '\n';
  report(warningMsg, 2);
}

void debug(string s){
  if(reportingLevel > 0) return;
  if(s.at(s.length()-1) == ')')
    indent++;//call
  string me = "";
  for(int i = 0; i<=indent; i++)
      me+=" ";
  me+=to_string((long long) indent)+" ";
  me+=s;    
  if(!hitEnd)
    me=me+": "+=sym->tokenText;
  report(me,-2);
  if(s.at(s.length()-1) == 'e')
    indent--;
}
//};
