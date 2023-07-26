/*
  Dakota
  1/1/19
 */

#ifndef LINTER_H
#define LINTER_H
#include "Flags.h"
#include <regex>


/*

This file will help the Lexer and possibly parser lintProg a Twine file
Things to check:

-indentation
-excess ( )
-names of vars, functions, classes (length, repeats, a_b or aB etc..)
-use of implicit conversions (int x = __ANY__)
-funk/method names
-length/complexity of methods
-length of lines
-use of FORCE
-use of auto returns(or just checks if you have returns in gen)
-use of new lines

*/

int lostScore = 0;
int totalComments = 1;
int totalLines = 1;
int concecutiveNewLines = 0;
int targetIndent = 0;
struct namedThing{//used to track if name or similar name previously used
  string name;
  string type;//func, var, class
  int lineUsed;
  namedThing(){};
  namedThing(string nm, string tp, int ln){name = nm;type=tp;lineUsed = ln;};
};
vector<namedThing> usedNames = vector<namedThing>();

void lintReport(string msg, int score_penalty){
  lostScore += score_penalty;
  //report(msg+" at line number "+to_string((long long) sym->line),2);
  report(to_string((long long) sym->line)+" : "+msg, 2);
}

/*string toLower(const string& s){//return lowercase version of string
  string lower = "";
  for(int i = 0; i<s.length();i++){
    lower += tolower(s[i]);
  }
  return lower;
  }*/
const string simplify(const string& str){
  string final; final.reserve(str.length());
  for(size_t i = 0; i < str.size(); ++i)
    if(isalpha(str[i])) final += str[i];
  return toLower(final);
}
void checkName(namedThing N){
  for(int i = 0; i<usedNames.size(); i++){
    if(simplify(N.name) == simplify(usedNames.at(i).name) && !(N.name == usedNames.at(i).name && N.type == "class")){//TODO check for other types of similar names
      lintReport("Name of "+N.type+" '"+N.name+"' has similar name to "+usedNames.at(i).type+" '"+usedNames.at(i).name+"' previusly used at line: "+to_string((long long) usedNames.at(i).lineUsed), 1);
      //return;
    }
  }
}

void checkName(string N, string T){
  namedThing nT = namedThing(N, T, sym->line);//makes new names thing
  checkName(nT);//check if it is similar to any existing names
  usedNames.push_back(nT);//add it to existing names
}

void checkBlock(bool newLine){//doesnt really check block
  if(getLintFlag("NEW_LINE_FOR_BLOCK") && !newLine){
    //lintProg
    lintReport("Missing new line at start of block",1);
  }
}
void checkEmpty(){
  if(getLintFlag("NO_EMPTY_BLOCKS")){
    //lintProg
    lintReport("Empty block",1);
  }
}
void checkFunk(Funk * F, bool checkReturn = true){
  if(getLintFlag("FUNCTION_COMPLEXITY") && F->complexity.total() > getLintFlag("FUNCTION_COMPLEXITY")){
    //lintProg
    lintReport("Function '"+F->name+"' has complexity greater then allowed(complexity of "+to_string((long long)F->complexity.total())+"/"+to_string((long long)getLintFlag("FUNCTION_COMPLEXITY"))+")",2);
  }
  if(checkReturn && getLintFlag("MISSING_RETURN_STATMENTS") && F->returnType.type != "void" && (F->funkBlock->Lines.size() == 0 || F->funkBlock->Lines.back()->type != "ret")){
    //lintProg
    lintReport("Function '"+F->name+"' has no return statement yet has return type of "+F->returnType.type,3);//TODO doesnt work with constructors
  }
}
void checkFunkName(Funk * F){
  if(getLintFlag("FUNCTION_NAME_MAX_LENGTH") && F->name.length() > getLintFlag("FUNCTION_NAME_MAX_LENGTH")){
    //lintProg
    lintReport("Function '"+F->name+"' has name longer then allowed(length of "+to_string((long long)F->name.length())+"/"+to_string((long long)getLintFlag("FUNCTION_NAME_MAX_LENGTH"))+")",1);
  }
  if(getLintFlag("FUNCTION_NAME_MIN_LENGTH") && F->name.length() < getLintFlag("FUNCTION_NAME_MIN_LENGTH")){
    //lintProg
    lintReport("Function '"+F->name+"' has name shorter then allowed(length of "+to_string((long long)F->name.length())+"/"+to_string((long long)getLintFlag("FUNCTION_NAME_MIN_LENGTH"))+")",2);
  }
  if(getLintFlag("FUNCTION_NAME_PROPER")){//TODO line number is wrong
    //lintProg
    if(getLintFlag("FUNCTION_NAME_PROPER") == 3 && !regex_match(F->name, regex("^[a-z][a-z0-9=+\\-%!@]*([A-Z][a-z0-9=+\\-%!@]*)*$"))){//check for aB
      lintReport("Function '"+F->name+"' has incorrectly formatted name(form is aB aka camel case)", 1);
    }
    if(getLintFlag("FUNCTION_NAME_PROPER") == 1 && !regex_match(F->name, regex("^([a-z0-9]+(_[a-z0-9]+)*)+$"))){//check for a_b
      lintReport("Function '"+F->name+"' has incorrectly formatted name(form is a_b aka stringing words)", 1);
    }
    if(getLintFlag("FUNCTION_NAME_PROPER") == 2 && !regex_match(F->name, regex("^([a-z0-9])+(_[A-Z0-9]+[a-z0-9]+)*$"))){//check for a_B
      lintReport("Function '"+F->name+"' has incorrectly formatted name(form is a_B aka why you do it like that?)", 1);
    }
  }
  if(getLintFlag("NO_SIMILAR_NAMES")){
    checkName(F->name, "function");
  }  
}

/*
  Checks objects name for length, similar names etc.
*/
void checkClassName(object * obj){
  if(getLintFlag("OBJECT_NAME_LENGTH_MIN") && getLintFlag("OBJECT_NAME_LENGTH_MIN") > obj->name.length()){
    //lintProg
    lintReport("Object '"+obj->name+"' has name shorter then allowed (length of "+to_string((long long)obj->name.length())+"/"+to_string((long long)getLintFlag("OBJECT_NAME_LENGTH_MIN"))+")",1);
  }
  if(getLintFlag("OBJECT_NAME_LENGTH_MAX") && getLintFlag("OBJECT_NAME_LENGTH_MAX") < obj->name.length()){
    //lintProg
    lintReport("Object '"+obj->name+"' has name longer then allowed (length of "+to_string((long long)obj->name.length())+"/"+to_string((long long)getLintFlag("OBJECT_NAME_LENGTH_MAN"))+")",1);
  }
  if(getLintFlag("OBJECT_NAME_PROPER")){
    if(getLintFlag("OBJECT_NAME_PROPER") == 3 && !regex_match(obj->name, regex("^[a-z0-9]+([A-Z0-9]+[a-z0-9]*)*$"))){//check for aB
      lintReport("Object '"+obj->name+"' has incorrectly formatted name(form is aB aka camel case)", 1);
    }
    if(getLintFlag("OBJECT_NAME_PROPER") == 1 && !regex_match(obj->name, regex("^([a-z0-9]+(_[a-z0-9]+)*)+$"))){//check for a_b
      lintReport("Object '"+obj->name+"' has incorrectly formatted name(form is aB aka camel case)", 1);
    }
    if(getLintFlag("OBJECT_NAME_PROPER") == 2 && !regex_match(obj->name, regex("^([a-z0-9])+(_[A-Z0-9]+[a-z0-9]+)*$"))){//check for a_B
      lintReport("Object '"+obj->name+"' has incorrectly formatted name(form is aB aka camel case)", 1);
    }    
  }
  if(getLintFlag("NO_SIMILAR_NAMES")){//checks if me and mE are bing used, this can be confusing
    checkName(obj->name, "class");
  }
  
}

/*
  Check for number of member functions and member vars
*/
void checkObject(object * obj){
  //memberVars memberFunks
  if(getLintFlag("OBJECT_MAX_FUNCTIONS") && getLintFlag("OBJECT_MAX_FUNCTIONS") < obj->memberFunks.size()){
    //lintProg
    lintReport("Object '"+obj->name+"' has more than allowed number of functions( "+to_string((long long)obj->memberFunks.size())+"/"+to_string((long long)getLintFlag("OBJECT_MAX_FUNCTIONS"))+")",1);
  }
  if(getLintFlag("OBJECT_MAX_VARS") && getLintFlag("OBJECT_MAX_VARS") < obj->memberVars.size()){
    //lintProg
    lintReport("Object '"+obj->name+"' has more than allowed number of member vars( "+to_string((long long)obj->memberVars.size())+"/"+to_string((long long)getLintFlag("OBJECT_MAX_VARS"))+")",1);
  }

  if(getLintFlag("OBJECT_NAME_PROPER")){
    //lintProg
    if(getLintFlag("OBJECT_NAME_PROPER") == 3 && !regex_match(obj->name, regex("^[a-z0-9]+([A-Z0-9]+[a-z0-9]*)*$"))){//check for aB
      lintReport("Object '"+obj->name+"' has incorrectly formatted name(form is aB aka cammel case)", 1);
    }
    if(getLintFlag("OBJECT_NAME_PROPER") == 1 && !regex_match(obj->name, regex("^([a-z0-9]+(_[a-z0-9]+)*)+$"))){//check for a_b
      lintReport("Object '"+obj->name+"' has incorrectly formatted name(form is a_b aka python rip-off)", 1);
    }
    if(getLintFlag("OBJECT_NAME_PROPER") == 2 && !regex_match(obj->name, regex("^([a-z0-9])+(_[A-Z0-9]+[a-z0-9]+)*$"))){//check for a_B
      lintReport("Object '"+obj->name+"' has incorrectly formatted name(form is a_B aka why you do it like that?)", 1);
    }
  }
}

/*
  Checks if all vars need a declaired type
*/
void checkTypeAllow(string varName, bool typeDefined = true){
  if(typeDefined && getLintFlag("VAR_TYPE_DEFINED"))
    //lintProg
    lintReport(varName+" was declaired without type being set, implicet type is __ANY__", 1);
}

void checkConvertion(string targetType, string resolvingType){//TODO
  if(getLintFlag("IMPLICIT_CONVERTIONS") && convertedType(targetType) != "*"){
    if(resolvingType == "string" && (targetType == "int" || targetType == "double")){//this allows implicit convertion from string to bool, aka check for empty string
      if(getLintFlag("IMPLICIT_CONVERTIONS") <= 3)//no implicit convertions from string to number(is length of string)
        lintReport("(hard) Implicit convertion from "+convertedType(resolvingType)+" to "+convertedType(targetType), 2);
    }else if(convertedType(targetType) != convertedType(resolvingType)){
      if(getLintFlag("IMPLICIT_CONVERTIONS") <= 2)//no convertions that are wierd, like int to string(string of 16 is "16")
	lintReport("Implicit convertion from "+convertedType(resolvingType)+" to "+convertedType(targetType), 1);
    }else if(targetType != resolvingType){
      if(getLintFlag("IMPLICIT_CONVERTIONS") == 1)//types dont match, very strict(no implicit calls from int to double)
	lintReport("(light) Implicit conversion from "+resolvingType+" to "+targetType, 1);
    }
  }
}

/*
  Check var length, previusly declaired similar names, and correct formatProg (aB VS a_b VS a_B)
*/
void checkVarName(string varName, bool isConstant = false){
  if(getLintFlag("VAR_MAX_LENGTH") && getLintFlag("VAR_MAX_LENGTH") < varName.length()){
    //lintProg
    lintReport("Var '"+varName+"' has name longer then allowed (length of "+to_string((long long)varName.length())+"/"+to_string((long long)getLintFlag("VAR_MAX_LENGTH"))+")",1);
  }
  if(getLintFlag("VAR_MIN_LENGTH") && getLintFlag("VAR_MIN_LENGTH") > varName.length()){
    //lintProg
    lintReport("Var '"+varName+"' has name shorter then allowed (length of "+to_string((long long)varName.length())+"/"+to_string((long long)getLintFlag("VAR_MIN_LENGTH"))+")",1);
  }
  if(getLintFlag("PROPER_VAR_NAMES")){
    //check-constants all caps, other vars,a_b or aB?
    if(isConstant && getLintFlag("CAPITALISE_CONSTANTS")){
      if(all_of(begin(varName), end(varName), [](char c){ return isupper(c); })){
	
      }else{
	//lintProg
	lintReport("Var '"+varName+"' is constant so should have fully capitalized name",1);
      }
    }else{
      if(getLintFlag("PROPER_VAR_NAMES") == 3 && !regex_match(varName, regex("^[a-z0-9]+([A-Z0-9]+[a-z0-9]*)*$"))){//check for aB
	lintReport("Var '"+varName+"' has incorrectly formatted name(form is aB aka camel case)", 1);
      }
      if(getLintFlag("PROPER_VAR_NAMES") == 1 && !regex_match(varName, regex("^([a-z0-9]+(_[a-z0-9]+)*)+$"))){//check for a_b
	lintReport("Var '"+varName+"' has incorrectly formatted name(form is a_b aka stringing words)", 1);
      }
      if(getLintFlag("PROPER_VAR_NAMES") == 2 && !regex_match(varName, regex("^([a-z0-9])+(_[A-Z0-9]+[a-z0-9]+)*$"))){//check for a_B
	lintReport("Var '"+varName+"' has incorrectly formatted name(form is a_B aka can't make up my mind)", 1);
      }
      //lintProg
    }
  }
  if(getLintFlag("NO_SIMILAR_NAMES")){
    checkName(varName, "var");
  }
}
void newLineAfterFunk(const Funk& f){
  if(getLintFlag("LINE_AFTER_FUNK") && !accept("\n")){
    lintReport("Expect new line after end of function "+f.name, 1);
  }
}
void checkLine(vector<TokenData> line){//checks if the length of the line is too long//should strings cound twords length?
  string me = "";me.reserve(line.size()*2);
  for(int i = 0; i<line.size();i++){//could also just check by adding length of string, but the current way allows more flexability in the future
    me+=line.at(i).tokenText;
  }
  if(getLintFlag("LINE_MAX_LENGTH") && getLintFlag("LINE_MAX_LENGTH") < me.length() ){
    lintReport("Line was too long ("+to_string((long long)me.length())+"/"+to_string((long long)getLintFlag("LINE_MAX_LENGTH"))+")",2);
  }
}

/*
Checks for max complexity of statement, number of bigAtoms used, and unnecessary use of ( )
*/
void checkExpression(expression3 * exp){
  if(getLintFlag("EXPRESSION_MAX_COMPLEXITY") && getLintFlag("EXPRESSION_MAX_COMPLEXITY") < exp->complexity){
    //lintProg
    lintReport("Expression exceeds max allowed complexity( "+to_string((long long)exp->complexity)+"/"+to_string((long long)getLintFlag("EXPRESSION_MAX_COMPLEXITY"))+")",2);
  }
  if(getLintFlag("EXPRESSION_MAX_LENGTH") && getLintFlag("EXPRESSION_MAX_LENGTH") < exp->bigAtoms->size()){
    //lintProg
    lintReport("Expression exceeds max length( "+to_string((long long)exp->bigAtoms->size())+"/"+to_string((long long)getLintFlag("EXPRESSION_MAX_LENGTH"))+")",2);
  }
  //check excess ()
  if(getLintFlag("EXCESS_PARENTHESES"))
    for(int i = 0;i<exp->bigAtoms->size();i++)
      if(exp->bigAtoms->at(i)->type == bigAtom::ATOM && exp->bigAtoms->at(i)->a->type == "exp3" && exp->bigAtoms->at(i)->a->exp3->bigAtoms->size() == 1){
	//lintProg
	lintReport("Found extra extra parentheses ( )", 1);
    }
}

void checkProg(prog * P){
  //not sure of anything to do here, but allows for easy checks in future 
  
}

double lintScore(){//lines is total number of lines in origonal file
  if(totalLines == 0) return 1.0; //empty program
  double rawScore = ((double)totalLines-lostScore)/totalLines;//between (hopefully 0) and 1
  return(rawScore)*10;
}

void printFunctionComplexity(Funk * f, bool indent){
  report("", 1);
  //report(string(indent?"  ":"")+"Function "+f->name+": "+to_string((long long)f->complexity.total()), 2);
  report(string(indent?"  ":"")+f->name+": "+to_string((long long)f->complexity.total()), 1);
  //if(f->complexity.funcCall)
  report(string(indent?"  ":"")+"  Function calls: "+to_string((long long)f->complexity.funcCall), 1);
  //if(f->complexity.loops)
  report(string(indent?"  ":"")+"  Loops:          "+to_string((long long)f->complexity.loops), 1);
  //if(f->complexity.conditionals)
  report(string(indent?"  ":"")+"  Conditionals:   "+to_string((long long)f->complexity.conditionals), 1);
  //if(f->complexity.exps)
  report(string(indent?"  ":"")+"  Expressions:    "+to_string((long long)f->complexity.exps), 1);
}

void printClassComplexity(object * obj){
  report(string("Class '")+obj->name+"':", 2);
  report(string(" Total Complexity: "+to_string((long long)obj->complexity())), 2);
  report(string(" Number of member vars: ")+to_string((long long)obj->memberVars.size()), 2);
  for(int i = 0; i<obj->memberFunks.size(); i++)
    printFunctionComplexity(obj->memberFunks.at(i), true);
  report("", 2);
}

void finalLint(){
  if(getLintFlag("NUMBER_OF_COMMENTS") && getLintFlag("NUMBER_OF_COMMENTS") < totalLines/totalComments){
    int lostPoints = (totalLines/totalComments) / getLintFlag("NUMBER_OF_COMMENTS");
    lintReport("Too few comments (1 per "+to_string((long long) totalLines/totalComments)+" lines / 1 per "+to_string((long long)getLintFlag("NUMBER_OF_COMMENTS"))+" lines)", lostPoints);
  }

  if(getLintFlag("SHOW_COMPLEXITIES")){
    report("Total program complexity: "+to_string((long long)Prog->totalComplexity()), 2);
    report("Number of global vars: "+to_string((long long)Prog->globalVars.size()), 2);
    if(Prog->classes.size()){
      report("----------------------Classes("+to_string(Prog->classes.size())+")------------------------", 2);
      for(int i = 0; i < Prog->classes.size(); i++)
	printClassComplexity(Prog->classes.at(i));
      report("----------------------End of classes---------------------", 2);
    }
      
    for(int i = 0; i < Prog->functions.size(); i++)
      printFunctionComplexity(Prog->functions.at(i), false);
    
  }
  
  report("__________________________________________________________________________________",2);
  report("| Number of parsable lines in program | "+to_string((long long) totalLines),2);
  report("| Total complexity of program         | "+to_string((long long) Prog->totalComplexity()),2);
  report("| Number of comments found            | "+to_string((long long) totalComments),2);
  report("| Number of points lost               | "+to_string((long long) lostScore),2);
  report("| Final linter score is               | "+to_string((long long) lintScore())+"/10\n", 2);
}
#endif //LINTER_H
