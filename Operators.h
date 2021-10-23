

#ifndef OPERATORS_H_
#define OPERATORS_H_

#include <string.h>
#include <vector>

#include "includes/__ANY__.h"
//#include "includes/TwineLib.h"

const char OPS[] = {'+','-','*','/','%','(','[','{','}',']',')','@','=','>','<','.','!', ':'};//]depends
const string OPS2[] = {"++","--","**","+=","-=","*=","/=","%=","==",">=","<=","!=","<<"};

const string RESERVED[] = {
                           //Original C
                           "auto", "const", "double", "float", "int", "short", "struct", "unsigned", "break",
			   "continue", "else", "for", "long", "signed", "switch", "void", "case", "default",
			   "enum", "goto", "register", /*"sizeof"--implimented as function*/
			   "typedef", "volatile", "char", "do", "extern",
			   "if", "return", "static", "union", "while",
			   
			   //C++
			   "asm", "dynamic_cast", "namespace", "reinterpret_cast", "try", "bool", "explicit",
			   "new", "static_cast", "typeid", "catch", "false", /*"operator",*/ "template", "typename",
			   "class", "friend", "private", "this", "using", "const_cast", "inline", "public",
			   "throw", "virtual", "delete", "mutable", "protected", "true", "typeid", "typename",
			   "wchar_t", 
			   
			   //C++11
			   "and", "bitand", "compl", "not_eq", "or_eq", "xor_eq", "and_eq", "bitor", "not", "or",
			   "xor", "alignas", "alignof", "constexpr", "const_cast", "decltype", "noexcept",
			   "export", //TODO finish

			   //TODO C++14/17
			   
			   //STDLIB
			   "endl", "INT_MIN", "std", "INT_MAX", "MAX_RAND", "NULL",
			   
			   //for Trans comp
			   "main", "argc", "argv",
			   
			   //for me
			   "func","C++","Cpp_code","global","super","DNE",

			   //special operators
			   //"in",
			   
                           //extra data types
                           "string","vector", "var",
};


const string functionKW[] = {
			     "open",//forces to be new thread
			     "closed"//forces to be on previus thread
};

const char num_OPS[] = {'+','-','*','/','%','.','>','<','^'};

const string bool_OPS[] = {"==",">=","<=","!=",">","<","and","or","&&","||","xor","!"};

const string tempBool[] = {"==",">=","<=","!=",">","<"};

const string binaryOPs[] = {"==",">=","<=","!=",">","<","and","or","&&","||","xor"};

const string afterOPs[] = {"++","--"};

const string beforeOPs[] = {"!","++","--"};

const string VARYTPE[] = {"int","double","string","void","bool","__ANY__"};//TODO add list?


/*
  nameSpace nameSpaces[] = {
  {"__ANY__",//toNum,toString}
};
 */

struct singleOP{
  string OP;
  int priority;
  bool frontable;
  bool endable;
  bool returnsBool;// ==, >=, < etc.
  bool leftToRightAssociativity;
  bool assignment;
};

//TODO order in order of usage
const singleOP Operators[] = {
			      
			      {"49", 100, false, false, false, true, false},
			      
			      {"@", 80, true, false, false, true, false},
			      
			      {"-=", 70, false, false, false, true, true},//TODO assignment?
			      {"+=", 70, false, false, false, true, true},
			      {"^=", 70, false, false, false, true, true},
			      {"*=", 70, false, false, false, true, true},
			      {"/=", 70, false, false, false, true, true},
			      {"%=", 70, false, false, false, true, true},
			      {"&=", 70, false, false, false, true, true},
			      {"|=", 70, false, false, false, true, true},
			      {"=", 70, false, false, false, true, true},
			      
			      {"<=", 70, false, false, true, true, false},
			      {">=", 70, false, false, true, true, false},
			      {">", 70, false, false, true, true, false},
			      {"<", 70, false, false, true, true, false},
			      
			      {"||", 60, false, false, true, true, false},
			      {"&&", 60, false, false, true, true, false},
			      {"xor", 60, false, false, true, true, false},
			      {"and", 60, false, false, true, true, false},
			      {"or", 60, false, false, true, true, false},
			      
			      
			      {"!=", 50, false, false, true, true, false},
			      {"==", 50, false, false, true, true, false},//tbh i still dont know how to do priorities

			      {"^", 45, false, false, false, true, false},
			      
			      {"*", 40, false, false, false, true, false},
			      {"/", 40, false, false, false, true, false},
			      {"%", 40, false, false, false, true, false},
			      
			      {"+", 30, false, false, false, true, false},
			      {"-", 30, true, false, false, true, false},
			      
			      
			      {"!", 20, true, false, true, false, false},//logical not
			      {"~", 20, true, false, false, false, false},//bitwise not
			      
			      {"++", 15, true, true, false, false, false},//incrament
			      {"--", 15, true, true, false, false, false},//decroment
			      
			      {".", 10, false, false, false, true, false},//member access
			      {"[", 10, false, false, false, true, false},
			      {"]", 10, false, false, false, true, false},
			      
			      //{":", 5, false, false, false, true, false},//cotton candy
};

singleOP getOP(const string OPName){
  for(int i  = 0;i<sizeof Operators/sizeof Operators[0];i++){
    if(Operators[i].OP == OPName)
      return Operators[i];
  }
  if(OPName == "::")
    return getOP(".");
  throw runtime_error("[getOP] Looking for OP that doesn exist: "+OPName);
};
struct symbol{//aka helpable object
  string name;
  string help;
  string toString()const{
    //string ret = name +" \t"+(name.length()>4?"":"\t")+help;
    string ret = name;
    for(int i=0;i<(16-name.length());i++) ret+=" ";
    ret+=help;
    return ret;
  }
};

const symbol symbols[] = {
			  {"bool", "An either true or false value"},
			  {"int", "A whole number that can take the vaue of -2147483648 to +2147483647"},
			  {"double","A decimal number"},//TODO
			  {"string","Any combination of numbers, symbols, and letters of any length"},
			  {"__ANY__","Can take on any value and any type, is the defualt type"}
};

struct preSetVar{
  string name;
  string type;
  string help;
  bool isConst;
  string toString()const{
    return (name+"\t\t"+(name.length()<10?("\t"):(""))+type+"\t  "+help);
  }
};

const preSetVar builtInValues[] = {
				   {"RUNNING_TIME",     "double","Time since the program started in seconds", true},//not constants, should be lower case?
				   {"CURRENT_TIME_MS",  "double","Current system time in milliseconds", true},
				   {"INT_MAX",          "int","Maximum value an integer can take on(+2147483647)", true},
				   {"INT_MIN",          "int", "Minimum value an integer can take(-2147483648)", true},
				   {"EXECUTION_TIME",   "double","Length of time the program has taken on the CPU in seconds (doesn't include time for sleep() or waiting for user input)", true},//in sec? ms?
				   {"PI",               "double","The value of PI (as defined by 3.14159265358979323846)", true},
				   {"twine",            "twine", "Used to force calls to the standard library", true},
};

struct functions{
  string name;
  //void *  fncPtr;
  string returnType;
  string params;
  string help;
  string alias;
  string toString()const{
    string ret = name+"("+params+")";
    short len = ret.length();
    for(int i=0;i<(66-len);i++) ret+=" ";
    ret+=help;
    return ret;
  }

};

const functions builtInFunctions[] = {//TODO finish and organize
				      {"abs","double","* value","Returns the absalute value of whatever is passed"},
				      {"exit","void","int exitStatus=0","Exits the program. A return status of 0 means succsess, other numbers represent some failure"},
				      {"fualt", "void", "void", "Creats an unrecoverable error"},
				      {"getenv", "string", "string varName", "Gets the value of the passed in enviromental variable from the system"},
				      {"getStack", "__ANY__", "void", "Returns list of the names of the functions on the stack"},//TODO replace with string[]
				      {"input","string","string msg=\"\"","Gets input from the user. Can pass a string to pass to the user first"},
				      {"insert", "void", "* list, int pos, * data", "Ineserts data into list and give position"},
				      {"len","int","__ANY__ target","Returns the length of a list passed, otherwise returns 1"},
				      {"list","__ANY__","__ANY__ item=0","Converts what is passed to a list"},
				      {"min","__ANY__","__ANY__ target","Returns the minimum of all values passed(passed as list)"},
				      {"max","__ANY__","__ANY__ target","Returns the maximum of all values passed(passed as list)"},
				      {"pow", "double", "double base=2.71828182845904523536, double exponent=1","Raises base to the exponent(base^exponent) with base defualt as the value e"},//TODO in TwineLib
				      {"print","void","* data=\"\", string text_color=\"\", string background_color=\"\"","Prints what is passed to it to the screen, then prints a new line"},
				      {"printn","void","* data, string text_color=\"\", string background_color=\"\"","Prints what is passed to the screen"},
				      {"printStack", "void", "void", "Prints stack trace"},
				      {"randInt","int","int min=0, int max=2147483647","Returns a random int between the two values passed"},
				      {"randDouble","double","double min=0, double max=1.0","Returns a random number betweent he two values passed"},
				      {"randStr","string","int length=10","Returns a random string the length of the value passed"},//TODO add chars to select from
				      {"round","int","double target","Rounds the number passed to the nearest whole number. Rounds .5 up to 1"},//TODO add rounding types(to the even, up, down, twoards 0, twords inf/-inf)
				      {"read","string","string fileName","Reads the file with the name passed and returns its contents"},//readFile?
				      {"sizeof", "int", "* data", "Returns the size, in bytes, of the object passed in"},
				      {"sleep","void","double seconds=1.0","Pausees the program for the number of seconds given"},
				      {"sort","__ANY__","* target","Sorts a list passed to it"},//TODO doesnt actually take *
				      {"sqrt","double","double data","Returns the sqrt of whatever is passed"},//TODO make general pow/root?
				      {"system","int","string command","Executes command on system"},
				      {"toInt","int","* data, int base=10","Converts what is passed to an integer. Can change the base by passing a second parameter"},
				      {"toDouble","double","* data, int base=10","Converts what is passed to a double"},//TODO add suport for base changes
				      {"toString", "string", "* data", "Converts what is passed in to a string"},
				      {"type", "string", "* data","Returns the type of the data passed in string form"},
				      {"write","bool","string fileName, string data, bool append=false","Writes the text passed to the file with the name passed: write(FILE_NAME, TEXT, APPEND?)"},

				      
};//e = 2.71828182845904523536

const functions interpFunctions[] = {//functions only found while in interpreting mode
				     {"clear","void","void", "Clears the screen"},
				     {"help", "void", "string what=\"\"", "Gets help about Twine interpreter"},
				     {"dumpVars","void","void","Deletes all defined vars"},
				     {"dumpFunks","void","void","Deletes all defined functions"},
				     {"listVars","void","void","Lists all var names/values"},//TODO should these return a string, print to screen, or both?
				     {"listFuncs","void","void","List all defined function names"},
				     {"stack", "__ANY__", "void", "Returns a list of strings of function names in the call stack"},
				     {"setReport", "void", "int level=2", "Sets reporting level/debuging level"},

};



struct nameSpace{
  string name;//name of namespace
  //string extends;//TODO
  string help;
  vector<functions> staticMethods;
  vector<functions> dynamicMethods;
  vector<preSetVar> classConstants;
  vector<preSetVar> classStaticVars;
  vector<preSetVar> classDynamicVars;
  vector<string> validOPs;
  //extends?
  string toString()const{
    string ret = name+": "+help+"\n";
    if(staticMethods.size()){
      ret+="Static methods: \n";
      for(int i = 0;i<staticMethods.size();i++){
	ret+="-"+staticMethods[i].name+":\n";
	ret+=staticMethods[i].toString();
      }
    }else
      ret+= "\n";
    if(dynamicMethods.size()){
      ret+="Dynamic methods: \n";
      for(int i = 0;i<dynamicMethods.size();i++){
	ret+=" -"+dynamicMethods[i].name+": \t"+dynamicMethods[i].toString()+"\n";
      }
    }else
      ret+= "\n";
    if(classConstants.size()){
      ret+="Class Constants: \n";
      for(int i = 0;i<classConstants.size();i++){
        ret+=" -"+classConstants[i].toString();
      }
    }else
      ret+= "\n";
    if(classStaticVars.size()){
      ret+="Class Static Vars: \n";
      for(int i = 0;i<classStaticVars.size();i++){
        ret+=" -"+classStaticVars[i].toString()+"\n";	
      }
    }else
      ret+= "\n";
    if(classDynamicVars.size()){
      ret+="Class Dynamic Vars: \n";
      for(int i = 0;i<classDynamicVars.size();i++){
        ret+=" -"+classDynamicVars[i].toString()+"\n";
      }
    }
    return ret;
  }
};

const string baseNameSym = "$";

const nameSpace nameSpaces[] = {
				{"string", "Any combination of numbers, symbols, and letters of any length",
				 {},//no static methods
				 {
				  //list of dynamic methods
				  {"at", "string", "int position=0", "Returns the character at the location passed in"},
				  {"length","int", "void","Returns length of the string, ie number of characters"},
				  {"find", "int", "string element", "Returns the position of the first matching occurance of element"},
				  {"append", "string", "string data", "Appends current string with passed in string"},
				  {"compair", "int", "string data", "Compairs two strings, 0=same string"},
				  {"substr", "string", "int start=0, int end=-1", "Returns the string between the the start and end points(inclusive)"},
				  {"insert", "string", "int position, string data", "Inserts the passed in data into the string at the position given"},
				 },
				 
				 {},//list of constants
				 {},//static vars
				 {},//dynamic vars
				 {},//OPS
				},
				
				{"int", "A whole number(no decmal) between -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807 (64bit)",
                                 {},//no static methods
                                 {//list of dynamic methods
                                 },

                                 {},//list of constants
                                 {},//static vars
                                 {},//dynamic vars
				 {},//OPS
                                },
				
				{"__ANY__", "Container for any other data type",
                                 {},//no static methods
                                 {
                                  //list of dynamic methods
				  {"append", "void", "__ANY__ newElement", "Turns current var into a list(if not already) and appends the new element to it"},
				  {"at", "__ANY__", "int position", "Returns the element at the position given"},
				  {"contains", "bool", "__ANY__ element", "Checks if the called on var  contains a specific element"},
				  {"dataAt", "__ANY__", "int position", "Returns a copy of the data stored at the position given"},
				  {"dataSize", "int", "void", "Rerturns the size, in bytes, of the data stored"},
				  {"del", "void", "int position=0", "Deletes the element in position provided"},
				  {"getType", "string", "void","Returns the type of data held in the var"},
				  {"sum", "double", "void", "Sums all the values in the var"},
				  {"length", "int", "void", "Returns the length of the data type held"},
				  {"size", "int", "void", "Returns 1 on int/double, or length for anything else"},
				  {"set", "void", "int position, __ANY__ data", "Sets the position with data"},
				  {"sub", "__ANY__", "int startingPosition=0, int endingPosition=0", "Returns the elements from start to end, inclusive"},//TODO-impliment
                                  {"toNum","double", "void","Returns number representation for container class(length of string, 0/1 for bool, etc)"},
				  {"toString","string","void","Returns string representation of contained data"},
				  {"in", "bool", "__ANY__ list", "Check if the called-on var is in the passed in list"},
				  //{"change", "__ANY__", "int position, __ANY__ data", "Set the position of the base to the value of data"},
				  //"popBack()", "popFront", appendFront()
                                 },

                                 {//list of constants
				  {"ZERO","int","Is equal to zero(0)"},
				 },
                                 {},//static vars
				 {
				  {"type", "string", "The hidden type of the varriable"},
				  {"ptr", "int", "The data held by the varriable"},
				  {"i", "int", "The integer data held by the var"},
				  {"b", "bool", "Boolean part of an __ANY__"},
                                 },//dynamic vars
				 {},//OPS
                                },

				{"double", "Any number, including decmals",
                                 {},//no static methods
                                 {
                                  //list of dynamic methods
                                 },

                                 {},//list of constants
                                 {},//static vars
                                 {},//dynamic vars
				 {},//OPS
                                },
				
				{"Object", "All classes extend object(it is the base class of all user-made objects)",
                                 {},//no static methods
                                 {
                                  //list of dynamic methods
				  {"toString","string","void","Returns string representation of contained data: defualt is object location"},
				  {"toNum", "double", "int base=10","Returns number representation of object: defualt is object location"},
                                 },

                                 {},//list of constants
                                 {},//static vars
                                 {},//dynamic vars
				 {},//OPS
                                },
				
				{"bool", "True or false values only",
				 {},//no static methods
				 {
                                  //list of dynamic methods
                                 },

                                 {},//list of constants
                                 {},//static vars
                                 {},//dynamic vars
				 {},//OPS
                                },
				{"runnable", "Makes thing multithreaded",//also known as branch
                                 {},//no static methods
                                 {
                                  //list of dynamic methods
				  {"receive", "*", "void", "Reads from channel"},
				  {"send", "void", "* data", "Writes to channel"},
				  {"pause", "void", "bool pause=true", "Pauses the thread"},
				  {"resume", "void", "void", "Resumes thread"},
				  {"safeEnd", "void", "void", "Ends the thread"},
				  {"end", "void", "void", "Ends the thread"},
				  {"forceEnd", "void", "void", "Ends the thread"},
				  {"sync", "void", "void", "Waits for thread to end"},
                                 },
				 
                                 {},//list of constants
                                 {},//static vars
                                 {
				  {"__run__", "bool", "If the thread/branch is currently set to run again after this cycle"},
				  {"__pause__", "bool", "If the thread/branch is currently set to pause at the beggining of the next cycle"},
				  {"__finish__", "bool", "If the thread has finished execution"},
				  {"__succsess__", "bool", "If the thread executed successfully"},
				 },//dynamic vars
                                 {},//OPS
				},
				 {"list", "Hold multiple of one type of object",
				  {},//no static methods
				  {
				   //list of dynamic methods
				   {"append", "void", "* data","Adds new element to the end of the list", "push_back"},
				   //{"appendFront", "void", "* data", "Adds new element to the front of the list", "push_front"},
				   {"begin", "int", "void", "Pointer to start of list"},
				   {"at", "*", "int position", "Returns dat at the givien position"},
				   {"resize", "void", "int size", "Makes the list a certain size"},
				   //{"insert", "void", "int position+$.begin()+, * data", "Inserts the given element at the given position"},
				   {"insert", "void", "int position,  * data", "Inserts the given element at the given position"},
				   {"clear", "void", "void", "Removes all elements in list"},
				   {"size", "int", "void", "Returns how many elements are in the list"},
				  },
				  
				  {},//list of constants
				  {},//static vars
				  {
				   {"type", "string", "Type of data inside the list"},
				  },//dynamic vars
				  {},//OPS
				  
				 },				
};

//a.insert(b,c) -> a.insert(a.begin()+b,c)

bool valipOP(const string& nameSpace, const string& OP){//TODO
  for(int i = 0; i< sizeof(nameSpaces)/sizeof(nameSpaces[0]); i++){
    if(nameSpace == nameSpaces[i].name){
      if(nameSpaces[i].validOPs.size() == 0)
	return true;      
      for(int j = 0; j<nameSpaces[i].validOPs.size();j++)
	if(nameSpaces[i].validOPs[j] == OP)
	  return true;
    }
  }
  return false;
}

bool validClassName(const string& name){
  //check namespaces
  for(int i = 0; i< sizeof(nameSpaces)/sizeof(nameSpaces[0]); i++){
    if(name == nameSpaces[i].name)
      return false;
  }
  if(name == "DNE")
    return false;
  return true;
}

bool validVarName(const string& name){
  //check built in var names
  //check keyWords
  //can't be DNE
  if(name == "DNE")
    return false;
  return true;
}
;

#endif/*OPERATORS_H_*/
