
#ifndef __OBJECT_H__
#define __OBJECT_H__

using namespace std;

#include <string>

class Object{//is base of all other objects
public:
  double toNum()const{return 0;}//could also return pointer value
  string toString()const{return "OBJECT @ "+to_string((long long)this);}//TODO make it hex
  Object(){;}
};

//dynamic object for sotring an object in __ANY__

#ifdef _NOT_IMP_

#ifndef __DYNAMIC_OBJECT__
#define __DYNAMIC_OBJECT__

#include <map>

class dynamicObject{
public:
  map<string, __ANY__> dynaicVars;    //specific to this object
  map<string, __ANY__> * staticVars;  //shared between all objects of this type
  string objectType;
  string varName;
  dynamicObject * parent;

  __ANY__& at(const string& var){
    map<string, __ANY__>::iterator it = staticVars->find(var);
    if(it != staticVars->end())
      return it->second;
    it = dynamicVars.find(var);
    if(it != dynaicVars.end())
      return it->second;
    //if not force, throw, otherwise, add key
    throw invalid_argument("Varriable '"+var+"' does not exist in object '"+objectType+"'");
  }
  
};

#endif //__DYNAMIC_OBJECT__
#endif //_NOT_IMP_

#endif//__OBJECT_H__
