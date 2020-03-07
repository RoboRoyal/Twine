/*
 *  interpScope.h
 *
 *  Holds vars while in interp mode
 *
 *  Created on: Dec 4, 2018
 *      Author: Dakota
 */
//#define INTERP_SCOPE_H_
#ifndef INTERP_SCOPE_H_
#define INTERP_SCOPE_H_

//includes
class interpSingleVAR{//or could just use var from parserNode
 public:
  string varName;//name used in program
  string ghostType;//type it is supposed to be
  string varType;
  __ANY__ data;//holder for all data
  bool constant;
  interpSingleVAR(){
    varName = "";
    varType = "";
    data = __ANY__(0);
    ghostType = "__ANY__";
  }
  interpSingleVAR(const string name, const string type, bool con, string gType, __ANY__ any){
    varName = name;
    varType = type;
    constant = con;
    ghostType = gType;//not sure if this is needed?
    data = any;
  }
  interpSingleVAR(const string name, __ANY__ any, const string gType){
    varName = name;
    ghostType = gType;
    data = any;
    constant = false;
    varType = gType;
  }
};
class interpScope{
 public:
  bool newVarScope;
  vector<interpSingleVAR> varsInScope;
  interpScope(){
    varsInScope = vector<interpSingleVAR>();
    newVarScope = false;
  }
};
class interpMe{
public:
  interpScope * currentVars;//add it to FOR, assignemnts wihtout type, globals?
  vector<interpScope *> * localVars;
  interpScope * globalVars;
  void cleanMeOrElseIllLeak(){
    if(currentVars) delete currentVars;
    if(localVars) delete localVars;
    if(globalVars) delete globalVars;
  }
  interpMe(){
    currentVars = new interpScope();
    localVars = new vector<interpScope *>();
    globalVars = new interpScope();
  }
  void peekScope(){//print out all vars in all scopes
    cout<<"Peeking scope; # of local vars: "<<currentVars->varsInScope.size()<<flush<<" # of other scopes: "<<localVars->size()<<endl;
    for(vector<interpSingleVAR>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){//prints loval vars
      cout<<"LocalVar: "<<(i)->varName<<" : "<<i->data.toString()<<" with ghost type: "<<i->ghostType<<" and real type: "<<i->data.getType()<<"\t";
    }
    cout<<"\nOther scopes"<<endl;
    for(vector<interpScope *>::reverse_iterator i = localVars->rbegin(); i != localVars->rend(); i++){//prints scoped vars
      cout<<"\nNext scope("<<((interpScope *)*i)->varsInScope.size()<<"): "<<endl;
      for(int j = 0; j < ((interpScope *)*i)->varsInScope.size(); j++){
	cout<<"Scoped var: "<<(*i)->varsInScope[j].varName<<" : "<<(*i)->varsInScope[j].data.toString()<<" with ghost type: "<<(*i)->varsInScope[j].ghostType<<"\t";;
      }
    }
    cout<<"\nGlobal"<<endl;
    for(vector<interpSingleVAR>::iterator i = globalVars->varsInScope.begin(); i != globalVars->varsInScope.end(); i++){//prints global vars
      cout<<"GlobalVar: "<<(i)->varName<<" : "<<i->data.toString()<<" with ghost type: "<<i->ghostType<<" ";
    }
    cout<<endl;
  }
  void pushScope(bool newVar = false){//going into new scope, deeper
    //cout<<"Pushing scope"<<endl;
    localVars->push_back(currentVars);
    currentVars = new interpScope();
    currentVars->newVarScope = newVar;//newVar is for pushing a scope to add new var value type
    
    //example:
    //string x = "hello"
    //{ int x = 0
  }
  
  int pullScope(){//goes down a sclope, ie }
    //cout<<"Pull scope"<<endl;
    if(localVars->size() == 0)
      cout<<"Either I messed up, or you messed up, really bad"<<endl;
    int numOfNewVarScopes = 0;
    /*do{
      numOfNewVarScopes++;
      //delete localVars->back();
      localVars->pop_back();
      currentVars = localVars->back();
    }while(currentVars->newVarScope);//TODO free memory
    */
    while(currentVars->newVarScope){
      delete currentVars;
      numOfNewVarScopes++;
      currentVars = localVars->back();
      localVars->pop_back();
    }
    delete currentVars;
    currentVars = localVars->back();
    localVars->pop_back();
    //cout<<"Pulled: "<<numOfNewVarScopes<<endl;
    return numOfNewVarScopes;
  }
  __ANY__ getVar(string varName){//TODO should return ptr value, NULL if not found

    /*cout<<"Trying to get var: "<<varName<<endl;
    if(!currentVars->varsInScope.size())
      cout<<"Trying to get var when there are no scopes1"<<endl;
    if(!localVars->size())
      cout<<"Trying to get var when there are no scopes2"<<endl;
    if(!globalVars->varsInScope.size())
      cout<<"Trying to get var when there are no scopes3"<<endl;
    cout<<"check 2"<<endl;*/
    
    for(vector<interpSingleVAR>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){//checks global vars
      if(i->varName == varName) return i->data;
    }
    //cout<<"check 3"<<endl;
    for(vector<interpScope *>::reverse_iterator i = localVars->rbegin(); i != localVars->rend(); i++){//checks local vars
      for(int j = 0; j < ((interpScope *)*i)->varsInScope.size(); j++){
	if((*i)->varsInScope.at(j).varName == varName)return (*i)->varsInScope[j].data;
      }
    }
    //cout<<"check 4"<<endl;
    for(vector<interpSingleVAR>::iterator i = globalVars->varsInScope.begin(); i != globalVars->varsInScope.end(); i++){//checks global vars
      if(i->varName == varName) return i->data;
    }
    //cout<<"check 5"<<endl;
    if(varName == "CURRENT_TIME_MS")
      return (double)chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    return "DNE";
  }
  
  //returns if new var was made
  bool setVar(const string name, __ANY__ data, bool global = false, string type = "__ANY__"){//TODO search if var exists first, and replace if it does
    report("Call to setVar: "+name+" with type "+type, -2);

    if(!global){
      for(vector<interpSingleVAR>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){//checks global vars
	if(i->varName == name){
	  i->data = data;
	  i->ghostType = type;
	  return false;
	}
      }
      for(vector<interpScope *>::reverse_iterator i = localVars->rbegin(); i != localVars->rend(); i++){//checks local vars
	for(int j = 0; j < ((interpScope *)*i)->varsInScope.size(); j++){
	  if((*i)->varsInScope.at(j).varName == name){
	    (*i)->varsInScope.at(j).data = data;
	    (*i)->varsInScope.at(j).ghostType = type;
	    return false;
	  }
	}
      }
    }else{
      for(vector<interpSingleVAR>::iterator i = globalVars->varsInScope.begin(); i != globalVars->varsInScope.end(); i++){//checks global vars
	if(i->varName == name){
	  i->data = data;
	  i->ghostType = type;
	  return false;
	}
      }
    }
    //else if not found, make a new one
    report("New var being added", -2);
    if(global){
      globalVars->varsInScope.push_back(interpSingleVAR(name,data,type));
    }else{
      currentVars->varsInScope.push_back(interpSingleVAR(name,data,type));
    }
    return true;
  }
  void newVar(string name, __ANY__ data, bool global = false, string type = "__ANY__"){
    report("Call to newVar: "+name+" with type "+type+" and data: "+data.toString(), -2);
    if(!global){
      for(vector<interpSingleVAR>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){//checks global vars
        if(i->varName == name){
          throw invalid_argument("Trying to make new var when a var with same name exist in the same scope: "+name);
        }
      }
    }
    if(global){
      globalVars->varsInScope.push_back(interpSingleVAR(name,data,type));
    }else{
      currentVars->varsInScope.push_back(interpSingleVAR(name,data,type));
    }
    report("done newVar() ", -2);
  }
  bool removeVar(string varName){
    //TODO
    for(vector<interpSingleVAR>::iterator i = currentVars->varsInScope.begin(); i != currentVars->varsInScope.end(); i++){//checks global vars
      if(i->varName == varName){currentVars->varsInScope.erase(i); return true;}
    }
    for(vector<interpScope *>::reverse_iterator i = localVars->rbegin(); i != localVars->rend(); i++){//checks local vars
    for(vector<interpSingleVAR>::iterator j = ((interpScope *)*i)->varsInScope.begin(); j != ((interpScope *)*i)->varsInScope.end(); j++){
      if(j->varName == varName){(*i)->varsInScope.erase(j);  return true;}
    }
    }
    /*for(vector<SingleVAR>::iterator i = globalVars->varsInScope.begin(); i != globalVars->varsInScope.end(); i++){//checks global vars
      if(i->varName == varName){globalVars->varsInScope.erase(i);  return true;}
      }*/
    return false;
  }
  /*********************Set up functions*******************/
  void setUpVars(){
    this->setVar("PI", 3.14159265358979323846, true, "double");
    this->setVar("OTHER_START_TIME", (double)chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count(), true, "double");
    //this->setVar("e",  2.718281828459045235360, true, "double");
    //this->setVar("credit", "RoboRoyal/Taco/Daber", true, "string");
    //this->pushScope();
  }
  
};


#endif//INTERP_SCOPE_H_
