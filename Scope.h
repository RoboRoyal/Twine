/*
 *  Scope.h
 *
 *  Created on: Sep 3, 2018
 *      Author: Dakota
 *
 *  Used to keep track of vars(name, type, const) in scope while parsing
 */

#ifndef SCOPE_H_
#define SCOPE_H_

#include "parserNodes.h"

class scope{
 public:
  bool newVarScope;
  //vector<SingleVAR> varsInScope;
  vector<var> varsInScope;
  scope(){
    //varsInScope = vector<SingleVAR>();
    varsInScope = vector<var>();
  }
};

scope * currentVars;//add it to FOR, assignemnts wihtout type, globals?
vector<scope *> * localVars;
scope * globalVars;

int numberOfScopes(){
  return localVars->size();
}
void peekScope(){
  cout<<"Peeking scope:"<<endl;
  for(vector<var>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){//checks global vars
    cout<<"LocalVar:\t"<<(i)->name<<" : "<<i->type<<"\t";
  }
  cout<<"\nother scopes"<<endl;
  for(vector<scope *>::reverse_iterator i = localVars->rbegin(); i != localVars->rend(); i++){//checks local vars
    cout<<"\tNext scope:"<<endl;
    for(int j = 0; j < ((scope *)*i)->varsInScope.size(); j++){
      cout<<"Scoped var:\t"<<(*i)->varsInScope[j].name<<" : "<<(*i)->varsInScope[j].type<<"\t";
    }
  }
  cout<<"Global"<<endl;
  for(vector<var>::iterator i = globalVars->varsInScope.begin(); i != globalVars->varsInScope.end(); i++){//checks global vars
    cout<<"GlobalVar:\t"<<(i)->name<<" : "<<i->type<<"\t";
  }

}
void pushScope(bool newVar = false){
  report("PushScope("+to_string(localVars->size()+1)+")", -2);
  localVars->push_back(currentVars);
  currentVars = new scope();
  currentVars->newVarScope = newVar;
}
int pullScope(){
  report("PullScope("+to_string((long long)localVars->size()-1)+")", -2);
  if(localVars->size() == 0)
    throw invalid_argument("Pulling scope with no scopes left to pull");
  int numOfNewVarScopes = 0;
  while(currentVars->newVarScope){
    delete currentVars;
    numOfNewVarScopes++;
    currentVars = localVars->back();
    localVars->pop_back();
  }
  delete currentVars;
  currentVars = localVars->back();
  localVars->pop_back();
  return numOfNewVarScopes;
}
string getVARType(const string& varName){
  for(vector<var>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){//checks global vars
    if(i->name == varName) return i->type;
  }
  for(vector<scope *>::reverse_iterator i = localVars->rbegin(); i != localVars->rend(); i++){//checks local vars
    for(int j = 0; j < ((scope *)*i)->varsInScope.size(); j++){
      if((*i)->varsInScope.at(j).name == varName) return (*i)->varsInScope[j].type;
    }
  }
  for(vector<var>::iterator i = globalVars->varsInScope.begin(); i != globalVars->varsInScope.end(); i++){//checks global vars
    if(i->name == varName) return i->type;
  }
  return "DNE";
}
bool isConstant(const string& varName){
  for(vector<var>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){//checks global vars
    if(i->name == varName) return i->constant;
  }
  for(vector<scope *>::reverse_iterator i = localVars->rbegin(); i != localVars->rend(); i++)//checks local vars
    for(vector<var>::iterator j = ((scope *)*i)->varsInScope.begin(); j != ((scope *)*i)->varsInScope.end(); j++)
      if(j->name == varName)
	return j->constant;
  
  for(vector<var>::iterator i = globalVars->varsInScope.begin(); i != globalVars->varsInScope.end(); i++){//checks global vars
    if(i->name == varName) return i->constant;
  }
  //cout<<"Checking if non-existant var is const: "<<varName<<endl;
  return false;
}
bool isLocal(const string& name){
  for(vector<var>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){
    if(name == i->name) return true;
  }
  return false;
}
bool isList(const string& varName){
  for(vector<var>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){//checks global vars
    if(i->name == varName) return i->isList;
  }
  for(vector<scope *>::reverse_iterator i = localVars->rbegin(); i != localVars->rend(); i++)//checks local vars
    for(vector<var>::iterator j = ((scope *)*i)->varsInScope.begin(); j != ((scope *)*i)->varsInScope.end(); j++)
      if(j->name == varName)
        return j->isList;

  for(vector<var>::iterator i = globalVars->varsInScope.begin(); i != globalVars->varsInScope.end(); i++){//checks global vars
    if(i->name == varName) return i->isList;
  }
  return false;
}
vector<unsigned long> getArr(const string& varName){
  for(vector<var>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){//checks global vars
    if(i->name == varName) return i->arr;
  }
  for(vector<scope *>::reverse_iterator i = localVars->rbegin(); i != localVars->rend(); i++)//checks local vars
    for(vector<var>::iterator j = ((scope *)*i)->varsInScope.begin(); j != ((scope *)*i)->varsInScope.end(); j++)
      if(j->name == varName)
        return j->arr;

  for(vector<var>::iterator i = globalVars->varsInScope.begin(); i != globalVars->varsInScope.end(); i++){//checks global vars
    if(i->name == varName) return i->arr;
  }
  //error?
  return vector<unsigned long>();
}
var getVar(const string& varName){
  for(vector<var>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){//checks global vars
    if(i->name == varName) return *i;
  }
  for(vector<scope *>::reverse_iterator i = localVars->rbegin(); i != localVars->rend(); i++)//checks local vars
    for(vector<var>::iterator j = ((scope *)*i)->varsInScope.begin(); j != ((scope *)*i)->varsInScope.end(); j++)
      if(j->name == varName)
        return *j;

  for(vector<var>::iterator i = globalVars->varsInScope.begin(); i != globalVars->varsInScope.end(); i++){//checks global vars
    if(i->name == varName) return *i;
  }
  return var("DNE", "DNE");
}
void setVar(const string& name, const string& type = "__ANY__", bool global = false, const bool constant = false, const bool isList = false){
  report("Setting var (complex): "+name+" with type: "+type+" Global: "+(global?"True":"False"),-2);
  if(global){
    globalVars->varsInScope.push_back(var(name, type, constant, isList));
  }else{
    currentVars->varsInScope.push_back(var(name, type, constant, isList));
  }
}
void setVar(const var& v, bool global = false){
  report("Setting var: "+v.name+" with type: "+v.type+" Global: "+(global?"True":"False"),-2);
  if(global){
    globalVars->varsInScope.push_back(v);
  }else{
    currentVars->varsInScope.push_back(v);
  }
}
bool removeVar(const string& varName){
  //TODO: should I allow removal of global vars?
   for(vector<var>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){//checks global vars
     if(i->name == varName){currentVars->varsInScope.erase(i); return true;}
  }
  for(vector<scope *>::reverse_iterator i = localVars->rbegin(); i != localVars->rend(); i++){//checks local vars
    for(vector<var>::iterator j = ((scope *)*i)->varsInScope.begin(); j != ((scope *)*i)->varsInScope.end(); j++){
      if(j->name == varName){(*i)->varsInScope.erase(j);  return true;}
    }
  }
  /*for(vector<SingleVAR>::iterator i = globalVars->varsInScope.begin(); i != globalVars->varsInScope.end(); i++){//checks global vars
    if(i->varName == varName){globalVars->varsInScope.erase(i);  return true;}
    }*/
  return false;
}

//--------------------------------------------

//Given a string, it will try to find what type of value it is
//First by checking if its a primitive type, then by checking if it is a variable
string valType(const string& s){
  if(s[0] == '"')
    return "string";
  if(s[0] == '1' ||s[0] == '2'||s[0] == '3'||s[0] == '4'||s[0] == '5'||s[0] == '6'||s[0] == '7'||s[0] == '8'||s[0] == '9'
     ||s[0] == '0'||s[0] == '.')
    return "num";
  if(s == "true" ||s=="false")
    return "bool";
  if(s[0] == '[')
    return "list";
  return getVARType(s);
}

void initDefualtVars(){
  setVar("args","__ANY__");
  for(int i =0;i<sizeof(builtInValues)/sizeof(builtInValues[0]);i++){
    setVar(builtInValues[i].name,builtInValues[i].type);
  }
}

void cleanScopes(){
  while(localVars->size())
    pullScope();
  delete currentVars;
  delete globalVars;
}

#endif//SCOPE_H_
