
#ifndef INTERP_H
#define INTERP_H
#include <exception>

//#include "Scope.h"
#include "2Parser.h"
#include "Lexer.h"
#include "parserNodes.h"
#include "includes/__ANY__.h"
#include "includes/TwineLib.h"
#include "includes/TypeInfoLib.h"
#include "_interpScope.h"

class returnExcep : public exception{
 public:
  __ANY__ data;
  //string type;
  string what(){
    return "Return EXP: "+data.toString();
  }
};
class returnExit : public exception{//used for exit();
public:
  int exitCode = 0;
  string what(){
    return "Exit exception with status: "+to_string((long long) exitCode);
  }
};

const string versionNumber = "null";
bool implicitPrint = true;
bool usedAssign = false;
//__ANY__ lastEval = __ANY__();//maybe i will imp?

vector<string> stackTrace;

int Interp(const string& = "");//Take flags?
void printWelcome();

string getInput();
int checkFlags(const string& line);
void initialVarSetUp();
int executeInterp(const string& data, const bool runMain = true);

bool interpProg(prog * prog);
__ANY__ interpFunk(Funk * f);
void interpBlock(Block * b, bool pushScope = true);
void interpLine(parseNode * pn);
void interpIF(IF * F);
void interpWHILE(WHILE * W);
void interpFOR(FOR * F);
void interpFOR2(FOR2 * F);

void interpVarAss(varAssign * va);
__ANY__ interpFunkCall(funkCall * fc);
__ANY__ interpUserFunkCall(Funk * f, funkCall * fc);

__ANY__ interpExpression(const expression2 * exp);
__ANY__ interpExpressionHelper(const expression2 * exp);
__ANY__ interpExpression(expression * exp);
__ANY__ interpExpression(expression3 * exp);
__ANY__ interpAtom(atom * a);

__ANY__ applyOP(__ANY__ left, const string& OP ,__ANY__ right);
__ANY__ applyOP(__ANY__ left, const string& OP, const string& name = "");
__ANY__ applyOP(const string& OP, __ANY__ right, const string& name = "");
__ANY__ interpDot(__ANY__& left, atom* right);

void cleanUp(prog * Prog);
void analizeProg(prog * Prog);

void interpTry(tryCatch * TC);
/*----------Interpreter only functions---------*/

void clear(){
  cout<<endl;
  int clr = system("clear");
}

void credits(){cout<<"Taco-time\n"<<endl;}

__ANY__ getStackTraceAsANY(){
  __ANY__ ret = list();
  for(int i = 0; i<stackTrace.size();i++)
    ret.append(stackTrace[i]);
  return ret;
}
bool dropFunction(string funkName){
  for(int i = 0; i<Prog->functions.size();i++){
    if(Prog->functions.at(i)->name == funkName){
      report("Found function for deletion at "+to_string((long long) i), -2);
      Prog->functions.erase(Prog->functions.begin() + i);
      return true;
    }
  }
  return false;//function name not found
}

bool dropClass(string className){
  for(int i = 0; i<Prog->classes.size();i++){
    if(Prog->classes.at(i)->name == className){
      report("Found class for deletion at "+to_string((long long) i), -2);
      Prog->classes.erase(Prog->classes.begin() + i);
      return true;
    }
  }
  return false;
}
bool dropVar(string varName){
  
  return false;
}
void interpHelp(string what){

  if(what.size() && what.at(0) == '\"')
    what.erase(remove(what.begin(), what.end(), '"'), what.end());
  
  if(what == ""){
    cout<<"Enter code on the command line. When you finish writing your code and press enter, the code will automatically be run\n";
    cout<<"Alternitivly, you could enter a specific function or symbol to learn more about it (example: `help(\"functions\")` or `help(\"symbols\")`)"<<endl;
    cout<<"You can also try the help in the main compiler. To do this, exit the interpreter with `exit`, the run `./twine -help`\n"<<endl;
    return;
  }
  if(what == "functions"||what=="funcs"){//list all functions
    for(int i = 0;i<sizeof(builtInFunctions)/sizeof(builtInFunctions[1]);i++)
      cout<<builtInFunctions[i].toString()<<"\n";
    for(int i = 0;i<sizeof(interpFunctions)/sizeof(interpFunctions[1]);i++)
      cout<<interpFunctions[i].toString()<<"\n";
    return;
   }else if(what == "symbols"){
     for(int i = 0;i<sizeof(builtInValues)/sizeof(builtInValues[1]);i++)
       cout<<builtInValues[i].toString()<<"\n";
     cout<<"\nData types: \n";
     for(int i = 0;i<sizeof(symbols)/sizeof(symbols[1]);i++)
       cout<<symbols[i].toString()<<"\n";
    return;
  }
   for(int i = 0;i<sizeof(builtInFunctions)/sizeof(builtInFunctions[1]);i++){//check functions in Twinelib
     if(builtInFunctions[i].name == what){
       cout<<builtInFunctions[i].toString()<<"\n";
       return;
     }
   }
   for(int i = 0;i<sizeof(interpFunctions)/sizeof(interpFunctions[1]);i++){//check interp only functions
     if(interpFunctions[i].name == what){
       cout<<interpFunctions[i].toString()<<"\n";
       return;
     }
   }
   for(int i = 0;i<sizeof(builtInValues)/sizeof(builtInValues[1]);i++){
     if(builtInValues[i].name == what){
       cout<<builtInValues[i].toString()<<"\n";
       return;
     }
   }
   for(int i = 0;i<sizeof(symbols)/sizeof(symbols[1]);i++){
     if(symbols[i].name == what){
       cout<<symbols[i].toString()<<"\n";
       return;
     }
   }
   cout<<"Unknown what in help: "<<what<<'\n';
   cout<<"Try -help with nothing else to look at more options"<<endl;
}


#endif //INTERP_H
