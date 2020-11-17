
#ifndef PARSER_NODES_H_
#define PARSER_NODES_H_

#include "utils/Twine_Utils.h"


static const string INT_NAME = "_I";
static const string STRING_NAME = "_S";
static const string DOUBLE_NAME = "_D";
static const string VECTER_NAME = "_V";
static const string ANY_NAME = "_A";
static int numberOfNodes = 0; //number of parseNodes used


//define all classes here:
class parseNode;       //master holder of other classes
class include;         //include external files(twine or C)
class objectCall;      //for holding a call to an object funtion ie. a.b() --not used
class var;             //info for holding var data(name, type, starting value, etc)
class boolStatment;    
class comment;         //line and block comments
class Block;           //vector of lines(parseNode) mostly
class FOR_IN;          
class WHILE;           
class IF;              //if/elif/else
class Funk;            //functions
class CLASS;//TODO
class funkCall;        //function name, and parameters
class atom;            //holds litteral, var, func call, or expression
class expression;      //old type of expression
class varAssign;       //type, var name, expression
class boolStatment;    //old, unused?
class FOR;             //old
class FOR2;            //new type of FORs
class expression2;     //old expression in type [atom/exp2] OP [atom/exp2]
class bigAtom;         //used of exp3, hold atom or op and type
class expression3;     //new expression, vector of bigAtoms and cast
class tryCatch;        //try{BLOCK}catch(VAR){BLOCK}
class object;          //object-implimented as class
class channel;         //used to communicate between threads
class branch;          //a main thread
//deconstructors
class prog;            //holds functions and classes

class parseNode{
 public:
  string type;
  string data;
  void * theThing;
  int ID;
  parseNode(){
    ID = numberOfNodes;
    numberOfNodes++;
  }
  string toString(int indent = 0)const;
  ~parseNode();//impilmented at bottom
};

class include{
public:
  string pathToFile;
  string fileName;
  string importedName;
  bool cTypeInclude;

  include(){
    pathToFile = "";
    fileName = "";
    importedName = "";
    cTypeInclude = false;
  }
  string toString()const{
    string ret = "INCLUDE("+pathToFile+" "+fileName+")";
    if(importedName != "")
      ret+= " as "+importedName;
    ret+="\n";
    return ret;
  }
};

class objectCall{
  atom * base;
  funkCall * call;
};

class var{
 public:
  bool priv;//needed?
  bool constant;
  bool STATIC;//needed?
  bool reference;
  bool atomic;//aka safe
  bool ptr;
  bool isList;
  bool isBegin;
  string name;
  expression3 * startingValue;
  expression2 * startingValue2; //currently unused
  
  string type;
  string secondaryType;
  vector<unsigned long> arr;//array parameters if there are any
  //var * secondVar;//used to nest [][][] or could be done with just an int to messure depth

  int lineDefinedOn;
  
  //vector of tokens, used for debugging?
  //store tokens as they are being used
  var(){
    STATIC = false;
    priv = false;
    constant = false;
    atomic = false;
    ptr = false;
    reference = false;
    isList = false;
    startingValue = NULL;
    startingValue2 = NULL;
    type = "__ANY__";
    secondaryType = "";
    arr = vector<unsigned long>();
    lineDefinedOn = -1;
  }
  var(const string& tp){
    STATIC = false;
    priv = false;
    constant = false;
    atomic = false;
    ptr = false;
    reference = false;
    isList = false;
    startingValue = NULL;
    startingValue2 = NULL;
    type = tp;
    secondaryType = "";
    arr = vector<unsigned long>();
    name = "";
    lineDefinedOn -1;
  }
  var(string nm, string tp){
    STATIC = false;
    priv = false;
    constant = false;
    atomic = false;
    ptr = false;
    reference = false;
    isList = false;
    startingValue = NULL;
    startingValue2 = NULL;
    name = nm;
    type = tp;
    secondaryType = "";
    arr = vector<unsigned long>();
    lineDefinedOn = -1;
  }
  var(const string& varName,const string& varType , bool con, bool list = false){
    STATIC = false;
    priv = false;
    constant = con;
    atomic = false;
    ptr = false;
    reference = false;
    isList = list;
    startingValue = NULL;
    startingValue2 = NULL;
    name = varName;
    type = varType;
    secondaryType = "";
    arr = vector<unsigned long>();
    lineDefinedOn = -1;
  }
  var(const string& varName,const string& varType , bool con, const vector<unsigned long>& a){
    STATIC = false;
    priv = false;
    constant = con;
    atomic = false;
    ptr = false;
    reference = false;
    isList = false;
    startingValue = NULL;
    startingValue2 = NULL;
    name = varName;
    type = varType;
    secondaryType = "";
    arr = a;
    lineDefinedOn = -1;
  }
  void clear(){
    STATIC = false;
    priv = false;
    constant = false;
    atomic = false;
    ptr = false;
    reference = false;
    isList = false;
    type = "";
    secondaryType = "";
    arr = vector<unsigned long>();
    lineDefinedOn = -1;
  }
  var(var * Var){
    STATIC = Var->STATIC;
    priv = Var->priv;
    constant = Var->constant;
    atomic = Var->atomic;
    ptr = Var->ptr;
    reference = Var->reference;
    startingValue = Var->startingValue;
    startingValue2 = Var->startingValue2;
    name = Var->name;
    type = Var->type;
    secondaryType = Var->secondaryType;
    arr = Var->arr;//TODO does this work?
    isList = Var->isList;
    lineDefinedOn = Var->lineDefinedOn;
  }
  //var(string nm,string tp,string sv){name = nm;type = tp;startingValue = sv;priv = false;constant = false;STATIC = false;}
  ~var();//implimented at bottom --TODO
  string toString()const{
    string ret = (constant?"cosnt ":"")+type;
    for(unsigned i =0; i < arr.size(); i++){
      ret += "[";
      if(arr[i] == 0){
	ret += "*";
      }else
	ret+= to_string(arr[i]);
      ret += "]";
    }
    ret+=" "+name;
    return ret;
  }
};

class comment{
public:
  bool singleLine;
  string text;
};

class Block{
public:
  vector<parseNode *> Lines;//TODO MAKE VECTOR OF PTRS
  Block(){
    Lines = vector<parseNode*>();
  }
  string toString()const{
    string me = "<BLOCK>";
    for(int i = 0; i < Lines.size();i++){
      me+=Lines.at(i)->toString();
    }
    me += "</BLOCK>";
    return me;
  }
  ~Block(){
    for(unsigned i = 0; i < Lines.size(); i++)
      delete Lines[i];
  }
};

class FOR_IN{
 public:
  //for i in listX 3:10{...}
  string itteratorName;//i
  string varName;//listX
  string start;//3
  string end;//10
  Block * blockState;//...
  bool vectorRange;//is it a number or a var we're itterating over
  FOR_IN(){
    itteratorName = "";
    varName = "";
    start = "0";
    end = "";
    blockState = new Block;
    vectorRange = false;
  }
};
class WHILE{
public:
  Block * blockState;
  //expression * exp;
  expression3 * exp3;
  bool isNorm;
  WHILE(){
    blockState = new Block;
    isNorm = true;
  }
  WHILE(bool a){
    blockState = new Block;
    isNorm = a;
  }
  ~WHILE();
  string toString()const;
};

class IF{
public:
  vector<Block*> * blockStatments;
  //vector<boolStatment *> * boolStatments;
  vector<expression3 *> * exps3;
  bool usingBool;
  IF(){
    usingBool = false;
    blockStatments = new vector<Block*>();
    //boolStatments = new vector<boolStatment*>();
    exps3 = new vector<expression3 *>();
  }
  ~IF();
  string toString()const;
};

class Funk{
public:
  string name;
  string alias;

  var returnType;
  vector<var> parameters;
  
  string dynamicParamType;//could just use var?
  string dynamicParamVar;
  
  //vector<string> TOKS;    
  bool open;
  bool closed;
  bool constant;
  Block * funkBlock;
  bool STATIC;
  /*complexity numbers to show how complex a function is, to determen when it should be made into a new thread*/
  struct complexity_struct{
    int funcCall=0, loops=0, conditionals=0, exps=0;
    int total()const{return (2+this->funcCall+this->loops+this->conditionals+this->exps);}
  }complexity;
  int totalComplexity()const{
    return this->complexity.funcCall + this->complexity.loops + this->complexity.exps;
  }
  Funk(){
    //TOKS = vector<string>();
    funkBlock = new Block();
    parameters = vector<var>();
    open = false;
    closed = false;
    STATIC = false;
    constant = false;
    returnType = var("__ANY__");
    dynamicParamType = "";
    //complexity = *(new complexity_struct());
    complexity.funcCall=0;complexity.loops=0;complexity.conditionals=0;complexity.exps=0;
  }
  string toString()const{
    string me = "Funk: ";
    if(open)
      me+= "OPEN";
    me += returnType.toString()+" "+name+"(";
    for(unsigned i = 0; i <parameters.size(); i++){
      me+= parameters[i].toString();
      if(i != parameters.size()-1)
	me+=", ";
    }
    me+=")  ("+ to_string((long long) complexity.total())+")\n";
    me+="\n";
    me+=funkBlock->toString();
    return me;
  }
  operator string(){
    string me = "";
    me+="Name: "+name+"\n";
    return me;
  }
  ~Funk(){
    delete funkBlock;
  }
}activeFunk;

class funkCall{
 public:
  string funkName;
  vector<expression3*> parameters;
  vector<expression2*> parameters2;
  atom * base;//unused
  bool newThreadMe = false;
  funkCall(){
    parameters = vector<expression3*>();
    funkName = "";
    base = NULL;
  }
  ~funkCall();
  string toString()const;
};

class atom{
 public:
  //var, function call, string(VALUE)
  string type;//what it is(exp3 = expressioon 3, funkCall = function call, lit = string(or num) litteral, objCall = obj.function()
  string cast;//casts applyed to it
  //string helper;//what type it is(used by parser)
  //vector<unsigned long> arrHelper;
  var helper;
  bool baseIsStatic;
  bool stringLit;
  //string base; string front; string end;
  union{
    string * literalValue;
    var * VAR;
    funkCall * fCall;
    //expression * exp;
    expression2 * exp2;
    expression3 * exp3;
  };
  atom(){
    baseIsStatic = true;
    type = "";
    cast = " ";
    helper = var("");
    stringLit = false;
    //front = ""; end = ""; base = "";
  }
  //atom(string a,string b,string c, expression* d){atom();type = a;cast=b;helper=c;exp=d;};
  atom(const atom& a){atom();type=a.type;cast=a.cast;helper=a.helper;VAR = a.VAR;}
  ~atom();//implimented at bottom
  
  string toString()const{
    /*string tmpOut = out;
    out = "";
    int tmp = reportingLevel;
    reportingLevel = 2;
    TransAtom(this);
    reportingLevel = tmp;
    string ret = out;
    out = tmpOut;
    return ret;*/
    string ret = "Type: "+type;
    if(type == "lit")
      ret+=" ; "+(*literalValue);
    return ret;
  }
  
};

class expression{//no longer used, depricated
 public:
  string type;
  string cast;
  vector<atom*> atoms;
  vector<string> OPs;
  //vector of atoms
  expression(){
    cast = "";
    type = "";
  }
};
class varAssign{
 public:
  var * VAR;
  //expression * exp;
  expression2 * exp2;
  expression3 * exp3;
  string OP;
  bool newVar;
  varAssign(){
    exp2 = NULL;
    exp3 = NULL;
    newVar = false;
    OP = "";
  }
  string toString()const;
  ~varAssign();
};
class boolStatment{
 public:
  string toString()const{return "/*not done*/";}
  vector<expression*> exps;
  vector<string> OPs;

};
/*class FOR{//TODO boolState? why?
 public:
  varAssign * lineOne;
  boolStatment * lineTwo;
  expression * lineThree;
  expression3 * exp3;
  Block * blockStatment;
  FOR(){
    lineOne = new varAssign;
    lineTwo = new boolStatment;
    lineThree = new expression;
    blockStatment = new Block;
  }
  };*/
class FOR2{
public:
  bool for_in;
  Block * blockExpression;
  union{
    struct{//regular FOR
      parseNode * lineOne;
      expression3 * lineTwo3;
      parseNode * lineThree;
    };
    struct{//For In
      var *  ittName;//could just be a string with var name
      bool isNum;
      expression * start;//start?
      expression3 * start3;
      union{
	var * ittVar;
	expression * end;
	expression3 * end3;
      };//union
    };//struct
  };//union
  string toString()const{
    return "/*FOR2--TODO*/\n";
  }
};//class

class expression2{
public:
  string cast;//toNum
  string OP;
  string helper;
  bool isStatic;
  bool leftIsExp = false, rightIsExp = false;
  union{
    atom * leftAtom;
    expression2 * leftExp;
  };
  union{
    atom * rightAtom;
    expression2 * rightExp;
  };
  expression2(){
    cast = "";
    helper = "";
    leftAtom = NULL;    rightAtom = NULL;
    leftIsExp = false;  rightIsExp = false;
  }
  
  ~expression2(){
    if(leftIsExp){
      if(leftExp){
	delete leftExp;
      }
    }else{
      if(leftAtom){
	delete leftAtom;
      }
    }
    if(rightIsExp){
      if(rightExp){
	delete rightExp;
      }
    }else{
      if(rightAtom){
	delete rightAtom;
      }
    }
  }
  string toString()const{//TODO
    /*string tmpOut = out;
    out = "";
    int tmp = reportingLevel;
    reportingLevel = 2;
    TransExp(this);
    reportingLevel = tmp;
    string ret = out;
    out = tmpOut;*/
    string left;
    if(leftAtom != NULL)
      if(leftIsExp)
	left = leftExp->toString();
      else
	left = leftAtom->toString();
    string right;
    if(rightAtom != NULL)
      if(rightIsExp)
        right = rightExp->toString();
      else
        right = rightAtom->toString();
    return left + OP + right;
  }
};

class bigAtom{
public:
  enum big_type{OP,ATOM, EXP3, EXP2}type;
  //union{//TODO i cant get string in union working
  string op;
  union{
    atom * a;
    expression3 * exp3;
    expression2 * exp2;
  };
  //};
  bigAtom(){type = OP; op = "";};
  bigAtom(atom * atom){type = ATOM; a = atom;};
  bigAtom(string operation){type = OP; op = operation;};
  bigAtom(expression3 * n){type = EXP3; exp3 = n;};
  bigAtom(expression2 * n){type = EXP2; exp2 = n;};
  string toString()const{
    if(type == OP)
      return "OP: "+op;
    if(type == ATOM)
      return "ATOM: "+a->toString();
    if(type == EXP3)
      return "EXP3 ";//+exp3->toString;
    return "EXP2: "+exp2->toString();
  }
  ~bigAtom();//implimented at bottom
};

class expression3{
public:
  int ptr;
  int complexity;
  string cast;
  vector<bigAtom*> * bigAtoms;
  expression3(){bigAtoms = new vector<bigAtom*>();complexity=0;cast = "";ptr = false;}
  ~expression3();
  string toString()const{
    return "/*exp3--TODO*/";
  }
};

class tryCatch{
public:
  Block * tryBlock;
  Block * catchBlock;
  //exception type
  string exceptionName;
  tryCatch(){tryBlock = new Block(); catchBlock = new Block(); exceptionName = "";}
  ~tryCatch(){
    //tryBlock->~Block();
    delete tryBlock;
    //catchBlock->~Block();
    delete catchBlock;
  }
  string toString()const{
    string ret = "TRY{\n";
    ret+=tryBlock->toString();
    ret+="\n}catch("+exceptionName+"){\n";
    ret+=catchBlock->toString()+"\n}\n";
    return ret;
  }
};
struct templateType{
  string name;
  string type;
};
class object{
public:
  bool isBranch;
  string name;
  string extends;
  //vecotr of member vars
  vector<var*> memberVars;
  vector<Funk*> constructors;
  //vector of member funks
  vector<Funk*> memberFunks;
  vector<object *> objectExtends;
  vector<templateType *> templates;
  ~object();
  object(){
    isBranch = false;
    name = "";
    extends = "";
    memberVars = vector<var*>();
    memberFunks = vector<Funk*>();
    constructors = vector<Funk*>();
    templates = vector<templateType *>();
  }
  int complexity()const{
    int total = (int) (memberVars.size()/10);
    for(int i = 0; i<memberFunks.size(); i++)
      total += memberFunks.at(i)->complexity.total();
    return total;
  }
  string toString()const{
    string me = "Class "+name+(extends==""? "": " ext. "+extends)+"("+to_string(complexity())+")\n";//TODO use objectExtends
    me+="Member Vars("+to_string(memberVars.size())+")\n";
    for(unsigned i = 0; i < memberVars.size();i++){
      me+=memberVars[i]->toString()+"\n";
    }
    me+="Constructors("+to_string(constructors.size())+")\n";
    for(unsigned i = 0; i < constructors.size();i++){
      me+=constructors[i]->toString()+"\n";
    }me+="Member Funks("+to_string(memberFunks.size())+")\n";
    for(unsigned i = 0; i < memberFunks.size();i++){
      me+=memberFunks[i]->toString()+"\n";
    }
    return me;
  }
};

class channel{
  string name;
  bool isConst;
  string type;
  //input and output are in relation to this thread
  bool input;
  bool output;
};

class branch: public object{ //is a thread automatically started when the program starts, isolated from the main thread
public:
  int branchType;
  string type;
  //some way to talk to main-or other-threads. something like Go's channels
  //vector<channel *> channels
  //vector<var*> memberVars; //private vars are more efficient since they dont have to be thread safe like global vars
  vector<var> parameters;
  vector<Funk*> memberFunks;
  Funk * write;
  Funk * read;
  
  branch():object(){
    write = NULL;
    read = NULL;
    parameters = vector<var>();
    memberFunks = vector<Funk*>();
    isBranch = true;
  }

  int complexity(){
    return 1;// + start->complexity.total();
  }
};

var::~var(){//currently cuases error so just let it out TODO
  /*if(startingValue)
    delete startingValue;
  if(startingValue2)
  delete startingValue2;*/
};

atom::~atom(){
  //cout<<"Atom deconstructor being called"<<endl;
  if(exp3 ==NULL)//check null ptr
    return;
  if(type == "exp3"){
    delete exp3;
  }else if(type == "exp2"){
   delete exp2;
  }else if(type == "lit"){
    delete literalValue;
  }else if(type == "funkCall"){
    delete fCall;
  }
}
parseNode::~parseNode(){
  //cout<<"Parser Node deconstructed"<<endl;
  //cout<<type<<" "<<theThing<<endl;
  if(theThing == NULL) return;
  if(type == "WHILE"){
    delete ((WHILE *)theThing);
  }else if(type == "IF"){
    delete((IF *)theThing);
  }else if(type == "FOR2"){
    delete((FOR2 *)theThing);
  }else if(type == "varAss"){
    delete ((varAssign *)theThing);
  }else if(type == "funkCall"){
    delete ((funkCall *)theThing);
  }else if(type == "exp3" ){//|| type == "ret" || type == "throw"){
    delete ((expression3 *)theThing);
  }else if(type == "exp2"){
    delete ((expression2 *)theThing);
  }else if(type == "TRY"){
    delete ((tryCatch *)theThing);
  }else if(type == "ret"){
    delete ((expression3 *)theThing);
  }//TODO
  //delete theThing;
  theThing = NULL;
}
string parseNode::toString(int indent)const{
  string me = "";
  for(int i=0; i < indent;i++)
    me+="  ";
  indent++;
  me += "PARSE_NODE("+to_string(ID)+")\n";
  //TODO
  if(type=="IF"){
    me+=((IF*)theThing) -> toString();
  }else if(type=="FOR2"){
    me+=((FOR2*)theThing) -> toString();
  }else if(type=="WHILE"){
    me+=((WHILE*)theThing) -> toString();
  }else if(type=="ret"){
    me+=((expression3*)theThing) -> toString();
  }else if(type=="TRY"){
    me+=((tryCatch*)theThing) -> toString();
  }else if(type=="throw"){
    me+=((expression3*)theThing) -> toString();
  }else if(type=="varAss"){
    me+=((varAssign*)theThing) -> toString();
  }else if(type=="funkCall"){
    me+=((funkCall*)theThing) -> toString();
  }else if(type == "exp3"){
    me+=((expression3*)theThing) -> toString();
  }else if(type=="C++"){
    me+=data;
  }else{
    me+="/* Type: "+type;
    me+=" Data: "+data+"*/";
  }
  me+='\n';
  indent--;
  return me;
}

funkCall::~funkCall(){
  for(unsigned i = 0; i < parameters.size(); i++){
    //delete parameters[i];//this gets mixed up with Funk pointer params. Uncomment leads to interp crashing
  }
  for(unsigned i = 0; i < parameters2.size(); i++){
    delete parameters2[i];
  }
}

bigAtom::~bigAtom(){
  if(type == ATOM){
    delete a;
  }else if(type == EXP3){
    delete exp3;
  }else if(type == EXP2){
    delete exp2;
  }    
}

expression3::~expression3(){
  for(unsigned i = 0; i < bigAtoms->size(); i++){
    delete bigAtoms->at(i);
  }
  delete bigAtoms;
}
IF::~IF(){
  for(unsigned i = 0; i < blockStatments->size();i++)
    delete blockStatments->at(i);
  delete blockStatments;
  for(unsigned i = 0; i < exps3->size();i++)
    delete exps3->at(i);
  delete exps3;
}
string IF::toString()const{
  string ret = "IF("+exps3->at(0)->toString()+"){\n";
  ret+=blockStatments->at(0)->toString()+"\n}";
  int x = 1;
  while(exps3->size() > x){
    ret+="ELSE_IF("+exps3->at(x)->toString()+"){\n";
    ret+=blockStatments->at(x)->toString()+"\n}";
    x++;
  }
  if(exps3->size() < blockStatments->size()){
    ret+="ELSE{\n";
    ret+=blockStatments->at(x)->toString()+"\n}";
  }
  return ret;
}
string WHILE::toString()const{
    string ret = "WHILE("+exp3->toString()+"){\n";
    ret+=blockState->toString();
    ret+="\n}\n";
    return ret;
}
WHILE::~WHILE(){
  delete exp3;
  delete blockState;
}
string funkCall::toString()const{//TODO
  string ret ="";
  if(base)
    ret+=base->toString();
  ret+=funkName+"(/*TODO*/)\n";
  return ret;
}
string varAssign::toString()const{
  string ret = VAR->toString();
  ret+=" "+OP+" ";
  if(exp2)
    ret+=exp2->toString();
  if(exp3)
    ret+=exp3->toString();
  return ret;
}
varAssign::~varAssign(){
  delete VAR;
  if(exp2)
    delete exp2;
  if(exp3)
    delete exp3;
}

object::~object(){
  for(unsigned i = 0; i < memberVars.size(); i++){
      delete memberVars[i];
  }
  for(unsigned i = 0; i < constructors.size(); i++){
      delete constructors[i];
  }
  for(unsigned i = 0; i < memberFunks.size(); i++){
      delete memberFunks[i];
  }
  for(unsigned i = 0; i < memberFunks.size(); i++){
    //delete memberFunks[i];//it crashes otherwise, so instead of fixing it, i just commented it out
  }
  for(unsigned i = 0; i < templates.size(); i++){
      delete templates[i];
  }
}

class prog{
public:
  vector<Funk *> functions;
  vector<object *> classes;
  vector<include *> globalIncludes;
  vector<var *> globalVars;
  vector<branch *> branches;
  //needed includes?
  //Funk * onExit; //if user defined, will always run, even if s aegfualt happens or exception is thrown
  prog(){
    functions = vector<Funk *>();
    classes = vector<object *>();
    globalIncludes = vector<include *>();
    globalVars  = vector<var *>();
    branches  = vector<branch *>();
  }
  int totalComplexity()const{
    int total = 0;
    for(int i = 0; i < functions.size(); i++)
      total+=functions.at(i)->complexity.total();
    
    for(int i = 0; i < classes.size(); i++)
      total+=classes.at(i)->complexity();

    for(int i = 0; i < branches.size(); i++)
      total+=branches.at(i)->complexity();
    
    total+=(int) globalVars.size()/10;
    return total;
  }
  
  string toString()const{
    string me = "\nProgram("+to_string(totalComplexity())+")\n";
    me+="Global Vars("+to_string(globalVars.size())+")\n";;
    for(unsigned i = 0; i < globalVars.size(); i++){
      me+=globalVars[i]->toString()+'\n';
    }
    me += "Funks("+to_string(functions.size())+")\n";
    for(unsigned i = 0; i < functions.size(); i++){
      me+=functions[i]->toString()+'\n';
    }
    me+="Classes("+to_string(classes.size())+")\n";
    for(unsigned i = 0; i < classes.size(); i++){
      me+=classes[i]->toString()+'\n';
    }
    me+="Branches("+to_string(branches.size())+")\n";
    for(unsigned i = 0; i < branches.size(); i++){
      me+=branches[i]->toString()+'\n';
    }
    me+="\n";
    return me;
  }

  ~prog(){
    for(unsigned i = 0; i < functions.size(); i++){
      delete functions[i];
    }
    for(unsigned i = 0; i < classes.size(); i++){
      delete classes[i];
    }
    for(unsigned i = 0; i < globalIncludes.size(); i++){
      delete globalIncludes[i];
    }
    for(unsigned i = 0; i < globalVars.size(); i++){
      delete globalVars[i];
    }
    for(unsigned i = 0; i < branches.size(); i++){
      delete branches[i];
    }
    //globalVars freed in 2Parser.cpp
  }
};

prog * Prog;
#endif //PARSER_NODES_H_ 
