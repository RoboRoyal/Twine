
#pragma once
#ifndef __ANY__H
#define __ANY__H

#include "EXTERN.h"
#include "CONFIG.h"

#include <sstream>

class __ANY__{//extend obj?
  //TODO split, encode?, zip
private:
  /*const static char STRING[];// = {'_', 'S', '\0'};
  const static char BOOL[];// = {'_', 'B', '\0'};
  const static char INT[];// = {'_', 'I', '\0'};
  const static char DOUBLE[];// = {'_', 'D', '\0'};
  const static char VECTOR[];// = {'_', 'V', '\0'};
  */
  
  const static string STRING;
  const static string BOOL;
  const static string INT;
  const static string DOUBLE;
  const static string VECTOR;
  //char to string needed in + //TODO

public:
  //__ANY__ runObjFunk(const __ANY__& obj, const string funkName, ...);
  //template <typename... Ts>
  __ANY__ runObjFunk(const string& funkName, ... );
  //{cout<<"running funk "<<funkName<<endl;return runObjFunk(*this, funkName);}
  __ANY__ runObjFunkHelper(const string& funcName, va_list& args );
  __ANY__ getObjVar(const __ANY__& obj, const string varName);
  __ANY__ getObjVar(const string varName){ return getObjVar(*this, varName);}
  union{void * ptr;bool b;int64_t i; double d;};

  string type;
  /*
    _I = int64_t
    _B = bool
    _D = double (64 bit)
    _S = * string
    _V = * vector<__ANY__> (as list)
    ------------------------------------
    NT = no type (defualt constructor)
    BT = bad type (has been derefrenced)
    ------------------------------------
    Possible future
    _F = * function
  */
  //TODO
  //all throws should be in TWINE_QUIET
  const static int64_t ZERO = 0;
  
  ~__ANY__(){
#ifdef TWINE_DEBUG
    cout<<"Deconsructor called for type: "<<this->type<<" at location(ptr): "<<this->ptr<<endl;
#endif //TWINE_DEBUG
    if(this->type == VECTOR){
      //cout<<"Deconsructor called for type: "<<this->type<<" at location(ptr): "<<this->ptr<<endl;
      //cout<<((vector<__ANY__>*)this->ptr)->size()<<endl;
      //cout<<"B"<<endl;
      if(this && ((vector<__ANY__>*)this->ptr)->size()){
	((vector<__ANY__>*)this->ptr)->clear();
	//((vector<__ANY__>*)this->ptr)->shrink_to_fit();
	//delete ((vector<__ANY__>*)this->ptr);
      }
      //cout<<"done dec"<<endl;
    }else if(this->type == STRING){
      //((string*)this->ptr)->clear();
      delete (string*)this->ptr;
    }else{
      //nothing for int/bool
    }
    type = "BT";//bad type now, need this?
  }
  
  //Defualt constructor
  __ANY__(){type = "NT";}
  //Regular constructors
  __ANY__(string tp, int in){type = tp;i = in;}
  __ANY__(int in){type = INT;i = in;}
  __ANY__(const string &s){type = STRING;ptr = new string(s);}
  __ANY__(const double dIn){type = DOUBLE;d = dIn;}
  __ANY__(const char c[]){type = STRING;ptr = new string(c);}
  __ANY__(const char c){type = STRING;const char tmp[1] = {c};ptr = new string(tmp);}
  __ANY__(const bool bin){type = BOOL;b = bin;}
  __ANY__(const long in){type = INT; i = in;}
  __ANY__(const long long in){
    type = INT; i = in;//if long long is 16, use double, else just use long
  }
  __ANY__(const size_t in){type = INT;i = in;}
  __ANY__(const vector<__ANY__> &data){
    type = VECTOR;
    ptr = new vector<__ANY__>(data);
  };
  __ANY__(const initializer_list<__ANY__>& data){
    type = VECTOR;
    ptr = new vector<__ANY__>(data);
  }

  /*template<typename T> __ANY__(const vector<T>& data){
    type = VECTOR;
    vector<__ANY__> * tmp = new vector<__ANY__>();
    for(int i = 0; i < data.size(); i++){
      tmp->emplace_back(__ANY__(data[i]));
    }
    ptr = tmp;
    }*/
  
  template<typename T> __ANY__(const vector<T>& data){
    type = VECTOR;
    vector<__ANY__> * tmp = new vector<__ANY__>();
    for(int i = 0; i < data.size(); i++){
      //tmp->emplace_back(__ANY__(data[i]));
      tmp->push_back(__ANY__(data[i]));
    }
    ptr = tmp;
  }

  template<typename T, int size> __ANY__(const array<T, size>& data){
    type = VECTOR;
    vector<__ANY__> * tmp = new vector<__ANY__>();
    for(int i = 0; i < size; i++){
      //tmp->emplace_back(__ANY__(data[i]));
      tmp->push_back(__ANY__(data[i]));
    }
    ptr = tmp;
  }
  
  //template class for any other data type
  template <typename T>
  __ANY__(T data){//cant be const &, sad face
    //cout<<"this is my call"<<endl;
    //type = type();
    type = typeid(data).name();
    __ANY__ a;
    if(type == typeid(a).name())
      throw invalid_argument("dont use this constructor for __ANY__ copy");//TODO remove when good
    ptr = &data;
  }
  
  __ANY__ operator= (const __ANY__& a){//TODO idk if i did this right
    if(!(this->type == "NT" || this->type == "BT")) this->~__ANY__();
    if(a.type == "NT" || a.type == "BT"){cout<<"Error in ANY"<<endl;      throw runtime_error("Cant make copy of type: "+a.type);}
#ifdef TWINE_DEBUG
    cout<<"Assignment constructor from type: "<<this->type<<" at location(ptr): "<<a.ptr<<endl;
#endif //TWINE_DEBUG
    this->type = a.type;
    if(type == INT){
      i = a.i;
    }else if(type == STRING){
      ptr = new string(*(string*)a.ptr);
    }else if(type == BOOL){
      b = a.b;
    }else if(a.type == DOUBLE){
      //ptr = new double(*(double*) a.ptr);
      d=a.d;
    }else if(a.type == VECTOR){//delete old data?
      //ptr = new vector<__ANY__>(*(vector<__ANY__>*)a.ptr);
      ptr = new vector<__ANY__>();
      //TODO for ....
      for(vector<__ANY__>::iterator it = ((vector<__ANY__>*)a.ptr)->begin(); it != ((vector<__ANY__>*)a.ptr)->end();it++)
        ((vector<__ANY__>*)ptr)->emplace_back(__ANY__(*it));
    }else{
#ifdef TWINE_DEBUG
      cout<<"ptr assigned"<<endl;
#endif //TWINE_DEBUG
      ptr = a.ptr;
    }
    return *this;
  }

  __ANY__(const __ANY__& a){//TODO copy assignment operator
    //cout<<"COPY "<<a.type<<endl;
    if(a.type == "NT" || a.type == "BT"){cout<<"Error in ANY"<<endl;      throw runtime_error("Cant make copy of type: "+a.type);}
    this->~__ANY__();//free allocated mem first?
    type = a.type;
#ifdef TWINE_DEBUG
    cout<<"Copy constructor from type: "<<this->type<<" at location(ptr): "<<a.ptr<<endl;
#endif //TWINE_DEBUG
    if(type == INT){
      i = a.i;
    }else if(type == STRING){
      ptr = new string(*(string*)a.ptr);
    }else if(type == BOOL){
      b = a.b;
    }else if(type == DOUBLE){
      d=a.d;
    }else if(type == VECTOR){
      ptr = new vector<__ANY__>();
      //TODO for ....
      for(vector<__ANY__>::iterator it = ((vector<__ANY__>*)a.ptr)->begin(); it != ((vector<__ANY__>*)a.ptr)->end();it++)
	((vector<__ANY__>*)ptr)->emplace_back(__ANY__(*it));
      
      //is this right?
    }else{
      if(a.ptr != NULL){
	ptr = a.ptr;//TODO call copy
      }else{//should i just allow this, with null ptr?
#ifndef TWINE_QUIET
        //throw invalid_argument("Trying to make new __ANY__ but type is unknown/pointer is null; type: "+a.type);
	if(a.type == "") throw invalid_argument("Trying to make new __ANY__ but type is blank");
#endif //TWINE_QUIET	
#ifdef TWINE_FORCE
	//this->type = BOOL;
	this->i=0;
#endif //TWINE_FORCE
      }
    }
  }//TODO

  /*bool castTo(class T){
    //#include <type_traits>
    //is_base_of
    type = typeid(T).name;
    ptr = static_cast<T *>(ptr);
    }*/

  int size(){
    if(type == BOOL || type == DOUBLE || type == INT)
      return 1;
    if(type == STRING)
      return ((string *) ptr)->length();
    if(type == VECTOR)
      return ((vector<__ANY__> *) ptr)->size();
    //else??
    //TODO add to num to other
    return -1;//TODO
  }
  void set(int position, const __ANY__& data){
    if(type == VECTOR){
      ((vector<__ANY__>*)this->ptr)->at(position) = data;
    }else if(type == STRING){
      ((string*)this->ptr)->at(position) = data.toString().at(0);
    }
    else if(type == BOOL){
      b = data.toNum();
    }
    else if(type == INT){
      i = data.toNum();
    }else if(type == DOUBLE){
      //(*(double *)this->ptr) = (*(double *)data.ptr);
      d = data.d;
    }
  }

  __ANY__ dataAt(int position){
    if(type == VECTOR){
      return ((vector<__ANY__>*)this->ptr)->at(position);
    }else if(type == STRING){
      return ((string*)this->ptr)->at(position);
    }else if(type == BOOL){
      return b;
    }else if(type == INT){
      return to_string(this->i).at(position);
    }else if(type == DOUBLE)
      //return to_string(*(double *)this->ptr).at(position);
      return to_string(d).at(position);
    return this->i;
  }
  __ANY__& at(int position){//TODO should return refrence
    if(type == VECTOR){
      return ((vector<__ANY__>*)this->ptr)->at(position);
    }/*
    else if(type == STRING){
      return ((string*)this->ptr)->at(position);
    }
    else if(type == BOOL){
      return b;
    }
#ifdef TWINE_WIERD_CONVERTIONS
    if(type == INT)
      return to_string(this->i).at(position);
    if(type == DOUBLE)
      return to_string(*(double *)this->ptr).at(position);
#endif //TWINE_WIERD_CONVERTIONS
    return this->i;
     */
    throw invalid_argument("Can not 'at' a non vector object, not implimented for type: "+this->getType());
  }
  void append(__ANY__ a){//TODO
    if(this->type == VECTOR){
      ((vector<__ANY__>*)this->ptr)->push_back(a);
      return;
    }else if(type == STRING){
      ((string *)ptr)->append(a.toString());
      return;
    }
    vector<__ANY__> * tmp = new vector<__ANY__>;
    tmp->push_back(__ANY__(*this));
    tmp->push_back(a);
    this->type=VECTOR;
    this->ptr = tmp;
  }
  void insert(__ANY__ a, int position){//TODO this and set have backwards parameters
    if(type == VECTOR){
      ((vector<__ANY__>*)this->ptr)->insert(((vector<__ANY__>*)this->ptr)->begin()+position, a);
    }else if(type == STRING){
      ((string*)this->ptr)->insert(position, a.toString());
    }else{
      //TODO
      //could trun bool or int to vector?
      if(type == INT){}
    }
  }//TODO
  //returns substring/part of array/part of the data
  __ANY__ sub(int start = 0, int size = 0, int step = 1){//TODO size vs end
    if(type == VECTOR){
      //size = end for this
      vector<__ANY__> vec = *(vector<__ANY__>*)this->ptr;
      if(start < 0)
	throw invalid_argument("Range error: start value must be greater than zero, "+to_string(start)+" given");
      if(start > size)
	throw invalid_argument("Start value ("+to_string(start)+" must be less then end value ("+to_string(size)+")");
      if(size>vec.size())
	throw invalid_argument("Range error: end value("+to_string(size)+") can not be greater than vector size("+to_string(vec.size())+")");
      if(!step)
	throw invalid_argument("Step size can not be zero");
      
      vector<__ANY__>  * newVec = new vector<__ANY__>();
      newVec->reserve(abs((size-start)/step));
      if(step > 0){//go forward
	for(;start < size; start+=step){//TODO add support for negitive step/start/end
	  newVec->push_back(vec[start]);
	}
      }else{
	for(;size>start; size+=step){
	  newVec->push_back(vec[size]);
	}
      }
      //cout<<"Vec"<<&vec<<" "<<&newVec<<endl;
      this->~__ANY__();
      this->type = VECTOR;
      this->ptr = newVec;
      return *this;
    }else if(type == STRING){
      return ((string*)this->ptr)->substr(start, size);
    }else if(type == INT){
      //TODO ?
      int t = i;
      for(int itt = start; itt>0;itt--)
	t/=10;
      int ret = 0;
      for(int itt = 0; itt<size;itt++){
	ret*=10;
	ret+=t%10;
	t/=10;
      }
      return ret;
    }else if(type == DOUBLE){
      //return stod(to_string((double)*((double *)this->ptr)).substr(start, size));
      return stod(to_string(d).substr(start, size));
    }else if(type == BOOL){
      //check wierd? TODO
      return b;
    }
#ifndef TWINE_QUIET
    throw invalid_argument("Invalid sub of invalid type "+this->getType());
#endif//TWINE_QUIET
    return 0;
  }//TODO
  void del(int position = 0){//TODO
    if(this->type == VECTOR){
      ((vector<__ANY__>*)this->ptr)->erase(((vector<__ANY__>*)this->ptr)->begin()+position);
    }else if(this->type == STRING ){
      ((string*)this->ptr)->erase(((string*)this->ptr)->begin()+position);
    }else{
#ifndef TWINE_QUIET//TODO if allow wierd stuff, do something for bool/int/double
      throw invalid_argument("Trying to delete element from object that isnt a list; type: "+this->getType());
#endif //TWINE_QUIET
    }
  }
  double sum() const{
    if(type != VECTOR)
      return this->toNum();
    //loop though vector, sum up
    double total = 0;
    for(unsigned i = 0; i<((vector<__ANY__>*)this->ptr)->size();i++){
      total += ((vector<__ANY__>*)this->ptr)->at(i).sum();
    }
    return total;
  }
  int length() const{
    if(this->type == INT)
      return log10(this->i)+1;//TODO more efficient way?
    if(this->type == DOUBLE)
      //return to_string(*(double *) this->ptr).size();//TODO efficint?
      return to_string(d).size();
    if(this->type == STRING)
      return((double)(*(string *)ptr).length());
    if(type == BOOL)
      return 1;
    if(type == VECTOR)
      return ((vector<__ANY__> *) ptr)->size();
#ifndef TWINE_QUIET    
    //throw invalid_argument("Invalid length check of invalid type "+this->getType());
#endif//TWINE_QUIET
    return 0;
  }
  
  bool in(const __ANY__& list)const{//TODO
    if(this->type != VECTOR && list.type == VECTOR){
      for(int i = 0; i<((vector<__ANY__>*)list.ptr)->size();i++){
	if((*this) == ((vector<__ANY__>*)list.ptr)->at(i))
	  return true;
      }
    }else if(this->type == STRING && list.type == STRING){
      return ((string*)list.ptr)->find(*(string*)this->ptr) >= 0;//or != -1
    }
    return (*this) == list;
  }
  bool contains(const __ANY__& element){
    if(this->type == VECTOR){
      for(vector<__ANY__>::size_type i = 0; i<((vector<__ANY__>*)this->ptr)->size();i++){
	if(((vector<__ANY__>*)this->ptr)->at(i) == element)
          return true;
      }
    }else if(this->type == STRING){
      return ((string*)this->ptr)->find(*(string*)element.ptr) != string::npos;
    }
    return (*this) == element;
  }
  string getType() const{
    if(this->type == INT){
      return "int";
    }else if((this->type == DOUBLE)){
      return "double";
    }else if((this->type == BOOL)){
      return "bool";
    }else if((this->type == STRING)){
      return "string";
    }else if((this->type == VECTOR)){
      return "list";
    }else if((this->type == "BT")){
      return "BAD_TYPE";
    }else if((this->type == "NT")){
      return "NO_TYPE";
    }
    return this->type;
  }
  double toNum(int base = 10) const{//TODO impliment base
    if(type == INT){//int
      return((double) i);
    }else if(type == DOUBLE){
      //return(*(double *) ptr);
      return d;
    }else if(type == STRING){//TODO
#ifndef TWINE_WIERD_CONVERTIONS
      throw invalid_argument("Invalid convertion from string to num. Use TWINE_WIERD_CONVERTIONS to allow this");      
#endif //TWINE_WIERD_CONVERTIONS
      
      return((double)(*(string *)ptr).length());//FIX this
    }else if(type == BOOL){
      if(b){
	return 1.0;
      }else{
	return 0.0;
      }
    }else if(type == VECTOR){
#ifndef TWINE_WIERD_CONVERTIONS
      throw invalid_argument("Invalid convertion from List to num. Use TWINE_WIERD_CONVERTIONS to allow this");
#endif //TWINE_WIERD_CONVERTIONS
      return ((vector<__ANY__> *) ptr)->size();
    }else if(type == "NT"){//no type, garbage data
      return 0;
    }else{
      //return (sizeof(ptr));//i know this is const,  just dont know what else to do, maybe return address(ptr)?
      return (long)ptr;
    }
  };
  string toString()const{
    //cout<<"__ANY__ toString()"<<endl;
    if(type == INT){//int
      return(to_string((long long) i));
    }else if(type == DOUBLE){
      /*string tmpStr = string(to_string((long double) (*(double *) ptr)));
      tmpStr.erase(tmpStr.find_last_not_of('0') + 1,-1);
      if(tmpStr[tmpStr.length()-1]=='.') tmpStr+='0';//or get rid of '.' as well?
      return tmpStr;*/
      ostringstream oss;
      //oss << (*(double *) ptr);
      oss<<d;
      string tmp = oss.str();
      if(tmp.find_first_of('.') == string::npos)
	tmp+=".0";
      return tmp;
    }else if(type == STRING){
      return(*(string *)ptr);
    }else if(type == BOOL){
      if(b){
	return "True";//or true/false
      }else{
	return "False";
      }
    }else if(type == VECTOR){
      vector<__ANY__> tmp = *(vector<__ANY__> *) ptr;
      string out = "[";
      for(vector<__ANY__>::iterator i = tmp.begin();i != tmp.end();){
	out+=i->toString();
	i++;
	if(i != tmp.end())out+=",";
      }
      out+="]";
      return out;
    }else{//TODO ast to type of object, then call toString() on it
      return type+"@"+to_string((long long) ptr);//TODO idk
      //return *(string *)(this->runObjFunk("toString").ptr);
    }
  };
  /*
    TODO Fix all operators so they dont make temp vars
    they should just set the new values to this
   */
  __ANY__ operator+(const __ANY__& a)const{//TODO bool
    __ANY__ b; 
#ifdef TWINE_DEBUG
    cout<<"Adding: "<<this->getType()<<" and "<<a.getType()<<endl;
#endif //TWINE_DEBUG
    if(this->type == INT){//int
      if(a.type == DOUBLE){
	b.type = DOUBLE;
	//b.ptr = new double(this->i+*(double *)a.ptr);
	b.d = this->i + a.d;
      }else if(a.type == INT){
	b.type = INT;
	b.i = this->i + a.i;
      }else if(a.type == BOOL){
#ifndef TWINE_WIERD_ASSIGNMENTS
	throw invalid_argument("Invalid addition between int and bool. To enable this, allow TWINE_WIERD_ASSIGNMENTS");
#endif //TWINE_WIERD_ASSIGNMENTS
	b.type = INT;
	if(a.b){
	  b.i = this->i + 1;
	}else{
	  b.i = this->i;
	}
      }else if(a.type == STRING){
#ifndef TWINE_WIERD_ASSIGNMENTS
	throw invalid_argument("Invalid addition between int and string. To enable this, allow TWINE_WIERD_ASSIGNMENTS");
#endif //TWINE_WIERD_ASSIGNMENTS
	b.type = STRING;
	b.ptr = new string(to_string((long long)this->i)+*(string*)a.ptr);
      }else if(a.type == VECTOR){
	b.type = VECTOR;
	vector<__ANY__> * tmp = new vector<__ANY__>;
	tmp->push_back(*this);
	for(vector<__ANY__>::iterator i = ((vector<__ANY__>*)a.ptr)->begin();i!=((vector<__ANY__>*)a.ptr)->end();i++){
	  tmp->push_back(__ANY__(*i));
	}
	b.ptr = tmp;
      }else{
#ifndef TWINE_QUIET
	throw invalid_argument("Invalid addition between int and IDK what: "+a.type);
#endif //TWINE_QUIET
      }
    }else if(this->type == DOUBLE){//double
      if(a.type == DOUBLE){//D+D
	b.type = DOUBLE;
	//b.ptr = new double(*(double *)this->ptr+*(double *)a.ptr);
	b.d = this->d + a.d;
      }else if(a.type == INT){//D+I
	b.type = DOUBLE;
	//b.ptr = new double(*(double *)this->ptr+a.i);
	b.d = this->d + a.i;
      }else if(a.type == BOOL){//D+B
#ifndef TWINE_WIERD_ASSIGNMENTS
	throw invalid_argument("Invalid addition between double and bool. To enable this, allow TWINE_WIERD_ASSIGNMENTS");
#endif //TWINE_WIERD_ASSIGNMENTS
	b.type = DOUBLE;
	if(a.b){
	  //b.ptr = new double((*(double *)this->ptr)+1);
	  b.d = this->d+1;
	}else{
	  //b.ptr = new double(*(double *)this->ptr);
	  b.d = this->d;
	}
      }else if(a.type == STRING){
#ifndef TWINE_WIERD_ASSIGNMENTS
        throw invalid_argument("Invalid addition between double and string. To enable this, allow TWINE_WIERD_ASSIGNMENTS");
#endif //TWINE_WIERD_ASSIGNMENTS
        b.type = STRING;
        //b.ptr = new string(to_string((long double)*(double *)this->ptr)+*(string*)a.ptr);
	b.ptr = new string(to_string((long double)this->d)+*(string*)a.ptr);
      }else if(a.type == VECTOR){//D+V
	b.type = VECTOR;
        vector<__ANY__> * tmp = new vector<__ANY__>;
        tmp->push_back(*this);
        for(vector<__ANY__>::iterator i = ((vector<__ANY__>*)a.ptr)->begin();i!=((vector<__ANY__>*)a.ptr)->end();i++){
          tmp->push_back(__ANY__(*i));
        }
        b.ptr = tmp;
      }else{
#ifndef TWINE_QUIET
        throw invalid_argument("Invalid addition between int and IDK what: "+a.type);
#endif //TWINE_QUIET
      }
    }else if(this->type == BOOL){//bool
      if(a.type == DOUBLE){
	b.type = DOUBLE;
        if(this->b){
          //b.ptr = new double((*(double *)a.ptr)+1);
	  b.d = a.d+1;
        }else{
          //b.ptr = new double(*(double *)a.ptr);
	  b.d = a.d;
        }
      }else if(a.type == INT){
	b.type = INT;
        if(this->b){
          b.i = a.i + 1;
        }else{
          b.i = a.i;
        }
      }else if(a.type == BOOL){
	if(this->b && a.b){
	  b.type = INT;
	  b.i = 2;
	}else if(this->b || a.b){
	   b.type = BOOL;
	   b.i = true;
	}else{
	  b.type = BOOL;
	  b.i = false;
	}
      }else if(a.type == STRING){
#ifndef TWINE_WIERD_ASSIGNMENTS
        throw invalid_argument("Invalid addition between string and bool. To enable this, allow TWINE_WIERD_ASSIGNMENTS");
#endif //TWINE_WIERD_ASSIGNMENTS
	b.type = STRING;
	if(this->b){
	  b.ptr = new string("True"+string(*(string*)a.ptr));
	}else{
	  b.ptr = new string("False"+string(*(string*)a.ptr));
	}
      }else if(a.type == VECTOR){
	b.type = VECTOR;
        vector<__ANY__> * tmp = new vector<__ANY__>;
        tmp->push_back(*this);
        for(vector<__ANY__>::iterator i = ((vector<__ANY__>*)a.ptr)->begin();i!=((vector<__ANY__>*)a.ptr)->end();i++)tmp->push_back(__ANY__(*i));
        
        b.ptr = tmp;
      }else{
#ifndef TWINE_QUIET
        throw invalid_argument("Invalid addition between int and IDK what: "+a.type);
#endif //TWINE_QUIET
      }
    }else if(this->type == STRING){
      if(a.type != VECTOR){//i think ive given up a little, forgive me
	b.type = STRING;
	b.ptr = new string(this->toString()+a.toString());
      }else{
	b.type = VECTOR;
        vector<__ANY__> * tmp = new vector<__ANY__>;
        tmp->push_back(*this);
        for(vector<__ANY__>::iterator i = ((vector<__ANY__>*)a.ptr)->begin();i!=((vector<__ANY__>*)a.ptr)->end();i++){
          tmp->push_back(__ANY__(*i));
        }
        b.ptr = tmp;
      }
    }else if(this->type == VECTOR){//TODO dont hink this does what i want it to
      b.type = VECTOR;
      vector<__ANY__> * tmp = new vector<__ANY__>;
      //tmp->push_back(*this);
      for(vector<__ANY__>::iterator i = ((vector<__ANY__>*)this->ptr)->begin();i!=((vector<__ANY__>*)this->ptr)->end();i++){
	tmp->push_back(__ANY__(*i));
      }
      tmp->push_back(a);
      b.ptr = tmp;
    }else{
#ifndef TWINE_QUIET
      throw invalid_argument("I know you're trying to do addition, but idk what these things are; LHS type: "+this->type+" RHS type: "+a.type);
#endif //TWINE_QUIET
    }
#ifdef TWINE_FORCE
    if(b.type = "BT"){//bad type
      b.type = INT;
      b.i = 0;
    }
#endif //TWINE_FORCE
    return b;
  }
  //TODO
/*string minus numer removes last # of chars, for vectors removes elements    
   */
/*------------------------------------------------------------------------------------------*/
  __ANY__ operator-(const __ANY__& a)const{//support for string remove substring
    __ANY__ b;
    if(this->type==STRING){//string
      if(a.type == STRING){//remov all instances of a from this, set to b
	b.type = STRING;
	string * tmp = new string(*(string*)this->ptr);
	//long long size = ((string*)a.data)->length();
	for(long long i = tmp->find(*(string*)a.ptr);i!=-1;i=tmp->find(*(string*)a.ptr))
	  tmp->erase(i, ((string*)a.ptr)->length());
	b.ptr=tmp;
      }else if(a.type == INT || a.type == DOUBLE|| a.type == BOOL){//remove last # of chars from this
	//TODO error check
#ifndef TWINE_QUIET
	long tmp = a.toNum();
	if(tmp > (*(string*)this->ptr).length())
	  throw invalid_argument("Can not subtract "+to_string(tmp)+" chars from string of length "+to_string((*(string*)this->ptr).length()));
#endif //TWINE_QUIET
	b.type = STRING;
	b.ptr = new string((*(string*)this->ptr).substr(0,(*(string*)this->ptr).length()-a.toNum()));//TODO
      }else{
#ifndef TWINE_QUIET
	throw invalid_argument("Trying to subtract invalid type: "+a.type+" from type string");
#endif //TWINE_QUIET
      }
    }else if(this->type == INT || this->type == DOUBLE){
      if(a.type == STRING){
	//if allow wierd type, this - stoi(a), otherwise throw
#ifndef TWINE_WIERD_ASSIGNMENTS
        throw invalid_argument("Invalid subtraction. Trying to subtract string from int. To enable this, allow TWINE_WIERD_ASSIGNMENTS");
#endif //TWINE_WIERD_ASSIGNMENTS
	b.type = this->type;
	if(this->type == DOUBLE){
	  try{
	    //b.ptr = new double (this->toNum() - stod(*(string*)a.ptr));
	    b.d = this->toNum() - stod(*(string*)a.ptr);
	  }catch(invalid_argument &ia){b.d = this->toNum(); }
	}else{
	  try{
	    //b.ptr = new double(this->toNum() - stoi(*(string*)a.ptr));
	    b.d = this->toNum() - stoi(*(string*)a.ptr);
	  }catch(invalid_argument &ia){b.i = (int) (this->toNum()); }
	}
      }else if(a.type == INT || a.type == DOUBLE){
	if(a.type == INT && this->type == INT){
	  b.type = INT;
	  b.i = this->i - a.i;
	  return b;
	}else{
	  b.type = DOUBLE;
	  //b.ptr = new double((*(double *)this->ptr)-(*(double *)a.ptr));
	  b.d = this->d - a.d;
	  return b;
	}
      }
    }else if(this->type == VECTOR){
      //TODO vector
      if(a.type == INT || a.type == DOUBLE){
	if(a.toNum() >= ((vector<__ANY__>*)this->ptr)->size()){
	  //TODO
	}else{
	  b.type = VECTOR;
	  //b.ptr = new vector<__ANY__>(((vector<__ANY__>*)this->ptr)->resize(((vector<__ANY__>*)this->ptr)->size()-a.toNum()));
	  vector<__ANY__> * tmp = (vector<__ANY__>*)this->ptr;
	  b.ptr = new vector<__ANY__>(tmp->begin(),tmp->end() - (int)a.toNum());
	  return b;
	}
      }else{

      }
    }else if(this->type == BOOL){
      b.type = BOOL;
      if(!this->b){
	b.b = false;
      }else{
	if(a.toNum()){
	  b.b = false;
	}else{
	  b.b = true;
	}
      }
    }else{
#ifndef TWINE_QUIET
      throw(invalid_argument("Trying to subtract invalid type: "+a.type+" from type (IDK): "+this->type));
#endif //TWINE_QUIET
    }
    return b;
  }
  __ANY__ operator*(const __ANY__& a)const {
    __ANY__ b;
    if(this->type==INT){//ints
      if(a.type==INT){
	b.type = INT;
	b.i=(this->i) * (a.i);
      }else if(a.type == DOUBLE){
	b.type=DOUBLE;
	//b.ptr=new double(this->i * (*(double *)a.ptr));
	b.d = this->i * a.d;
      }else if(a.type == BOOL){
	b.type = INT;
	b.i=(this->i * a.b);
      }else if(a.type==STRING){
	string * tmp = new string();
        for(int times = 0; times!=this->i;times++){
          (*tmp) += *(string*)a.ptr;
        }
        b.type = STRING;
        b.ptr = tmp;
      }else if(a.type == VECTOR){
	vector<__ANY__> * tmp = new vector<__ANY__>;
	for(int k = 0; k<this->i;k++){
	  for(vector<__ANY__>::iterator i = ((vector<__ANY__>*)a.ptr)->begin();i!=((vector<__ANY__>*)a.ptr)->end();i++)tmp->push_back(__ANY__(*i));
	}
	b.type = VECTOR;
	b.ptr = tmp;
      }else{
      }
    }else if(this->type == DOUBLE){//doubles
      if(a.type==INT){
	b.type = DOUBLE;
	//b.ptr = new double(*(double *)this->ptr * a.i);
	b.d = this->d * a.i;
      }else if(a.type == DOUBLE){
	b.type = DOUBLE;
        //b.ptr = new double(*(double *)this->ptr * (*(double *)a.ptr));
	b.d = this->d * a.d;
      }else if(a.type == BOOL){
        b.type = DOUBLE;
        //b.ptr=new double(*(double *)this->ptr * a.b);
	b.d = this->d * a.b;
      }else if(a.type==STRING){
	string * tmp = new string();
        for(int i = 0; i!=(int)this->d;i++){
          (*tmp)+=*(string*)a.ptr;
        }
        b.type = STRING;
        b.ptr = tmp;
      }else if(a.type == VECTOR){
	vector<__ANY__> * tmp = new vector<__ANY__>;
        for(int k = 0; k<(int)this->i;k++){
          for(vector<__ANY__>::iterator i = ((vector<__ANY__>*)a.ptr)->begin();i!=((vector<__ANY__>*)a.ptr)->end();i++)tmp->push_back(__ANY__(*i));
        }
        b.type = VECTOR;
        b.ptr = tmp;
      }else{
      }

    }else if(this->type == BOOL){//bools
      if(a.type==INT){
        b.type = INT;
        b.i=(this->b * a.i);
      }else if(a.type == DOUBLE){
        b.type=DOUBLE;
        //b.ptr=new double(this->b * (*(double *)a.ptr));
	b.d = this->b * a.d;
      }else if(a.type == BOOL){
        b.type = BOOL;
        b.b=(this->b && a.b);
      }else if(a.type==STRING){
	throw invalid_argument("not implimented");
      }else if(a.type == VECTOR){
	vector<__ANY__> * tmp = new vector<__ANY__>;
	if(this->b){
	  for(vector<__ANY__>::iterator i = ((vector<__ANY__>*)a.ptr)->begin();i!=((vector<__ANY__>*)a.ptr)->end();i++)tmp->push_back(__ANY__(*i));
	}else{
	  //nuthin
	}
	b.type = VECTOR;
        b.ptr = tmp;
      }else{
      }
    }else if(this->type == STRING){//strings//TODO negitives should reverse strings and vectors
      if(a.type == INT||a.type==DOUBLE||a.type==BOOL){
	string * tmp = new string();
	for(int i = 0; i!=(int)a.toNum();i++){//TODO efficency?
	  (*tmp)+=this->toString();
	}
	b.type = STRING;
	b.ptr = tmp;
      }else if(a.type==STRING){
	
      }else if(a.type == VECTOR){

      }else{
      }
    }else if(this->type == VECTOR){//vectors
      if(a.type == INT || a.type == DOUBLE||a.type == BOOL){
	vector<__ANY__> * tmp = new vector<__ANY__>;
	for(int k = 0; k<(int)a.toNum();k++){//todo
	  for(vector<__ANY__>::iterator i = ((vector<__ANY__>*)this->ptr)->begin();i!=((vector<__ANY__>*)this->ptr)->end();i++)tmp->push_back(__ANY__(*i));
	}
	b.type = VECTOR;
	b.ptr = tmp;
      }
    }else{
      
    }
    return b;
    }
  __ANY__ operator/(const __ANY__& a)const{
    __ANY__ b;
    if(a.toNum() == 0)
      throw invalid_argument("Can not devide by zero");
    if(this->type == INT){
      if(a.type == INT){
	b.type = INT;
	b.i = this->i/a.i;
      }else if(a.type == DOUBLE){
	b.type = DOUBLE;
        b.d = ((double)this->i)/a.d;
      }else{
	//not imp
      }
    }else if(this->type==DOUBLE){
      if(a.type == INT){
        b.type = DOUBLE;
        b.d = this->d/a.i;
      }else if(a.type == DOUBLE){
        b.type = DOUBLE;
        //b.ptr = new double((*(double *)this->ptr)/(*(double *)a.ptr));
	b.d = this->d / a.d;
      }else{
        //not imp
      }
    }else if(this->type==BOOL){
      b.type = BOOL;
      if(a.toNum() > 1){
	b.b = false;
      }else{
	b.b = this->b;
      }
    }else if(this->type==STRING){
      b.type = STRING;
      if(a.type == INT||a.type == DOUBLE){
	
	if(a.toNum() > 1){
	  string * tmp = new string((*(string*)this->ptr).substr(0,tmp->size()/a.toNum()));
	  b.ptr = tmp;
	}else if(a.toNum() < 1){
	  string * tmp = new string(((*(string*)this->ptr).substr(tmp->size()/a.toNum(),tmp->size())));
          b.ptr = tmp;
	}
      }
    }else if(this->type==VECTOR){
      b.type = VECTOR;
      if(a.type == INT||a.type == DOUBLE){
        if(a.toNum() > 1){
	  vector<__ANY__> * tmp = new vector<__ANY__>;
	  int len = ((vector<__ANY__>*)this->ptr)->size();
	  for(int i = 0;i<(len/a.toNum());i++){
	    tmp->push_back(((vector<__ANY__>*)this->ptr)->at(i));
	  }
          b.ptr = tmp;
        }else if(a.toNum() < 1){
	  vector<__ANY__> * tmp = new vector<__ANY__>;
	  for(double i = ((vector<__ANY__>*)this->ptr)->size();i>=0;i=i-a.toNum()){
            tmp->push_back(((vector<__ANY__>*)this->ptr)->at(i));
          }
          b.ptr = tmp;
        }
      }
    }else{//TODO
    }
    return b;
  }


  __ANY__ operator%(const __ANY__& a)const{
    __ANY__ b;
    if(this->type == INT){
      
      //TODO throw error if string/vec if not enabled
      if(a.type == INT||a.type == BOOL){
	b.type = INT;
	//b.i=(this->i)%(int)a.toNum();
	b.i = fmod(this->i, a.toNum());
      }else{
	b.type = DOUBLE;
	//b.d = this->i % (int)a.toNum();
	b.d = fmod(this->i, a.toNum());
      }
    }else if(this->type == DOUBLE){
      //TODO throw error if string/vec if not enabled
      b.type = DOUBLE;
      //b.d = this->i%(int)a.toNum();
      b.d = fmod(this->d, a.toNum());
    }else if(this->type == STRING){
      //TODO throw error if string/vec if not enabled
      string tmp2 = "";
      if(a.toNum() == 0) throw(invalid_argument("Divide by 0"));
      if(a.toNum()>0){
	for(double i = 0;i<((string*)this->ptr)->length();i=i+a.toNum()){
	  tmp2+=((*(string*)this->ptr)[((int)i)]);
	}
      }else{
	for(double i =((string*)this->ptr)->length();i>=0;i=i-a.toNum()){
	  tmp2+=((*(string*)this->ptr)[((int)i)]);
	}
      }
      string * tmp = new string(tmp2);
      b.type = STRING;
      b.ptr = tmp;
    }else if(this->type == VECTOR){
      if(a.toNum() <= 0) throw(invalid_argument("Divide by 0"));
	b.type = VECTOR;
	vector<__ANY__> * tmp = new vector<__ANY__>;
	for(double i = 0;i<((vector<__ANY__> *)this->ptr)->size();i=i+a.toNum()){
	  tmp->push_back((((vector<__ANY__> *)this->ptr)->at((int)i)));
	}
	b.ptr = tmp;//TODO
    }
    return b;
  }
 
  __ANY__ operator+=(const __ANY__& a){return (*this)=(*this)+a;}
  //void operator+=(__ANY__ a){(*this)=(*this)+a;}//TODO should this return?
  __ANY__ operator-=(const __ANY__& a){return (*this)=(*this)-a;}
  __ANY__ operator*=(const __ANY__& a){return (*this)=(*this)*a;}
  __ANY__ operator/=(const __ANY__& a){return (*this)=(*this)/a;}
  __ANY__ operator%=(const __ANY__& a){return (*this)=(*this)%a;}
  __ANY__ operator^=(const __ANY__& a){return (*this)=(*this)^a;}
  __ANY__ operator++(const int i){//TODO-these dont work and are inefficent
    if(type == VECTOR){
      //return 1+vector
      ((vector<__ANY__> *)this->ptr)->push_back(__ANY__(1));
      return (*this);
      /*}else if(type == STRING){
      //free memory?
      //return this->ptr = new string( "1" + (*(string *)this->ptr));
      return (*this) = 1+*/
    }else if(type == BOOL){
      return *this = (this->b=true);
    }else if(type == INT){
      this->i++;
      return *this;
    }else{
      return *this = (*this)+1;
    }
  }
  __ANY__ operator++(){
    //TODO
    if(this->type == INT){
      return *this = (this->i)+1;
    }else if(this->type == BOOL){
      return *this = (this->b=true);//why this way?
    }else{
      //cout<<"ERROR not implimented!(__ANY__ ++)"<<endl;
      //return __ANY__(0);
      return *this = (*this)+__ANY__(1);
    }
  }
  __ANY__ operator--(int i){
    //return *this = (*this)-1;//or can it be done more efficently other ways?
    return (*this) = (*this) - __ANY__(1);//TODO
  }
  
  __ANY__ operator--(){//could just return (*this)-1?
    if(this->type == STRING){//remove first char
      //try{((string *)this->ptr)->erase(0,1);}catch(exception e){return *this;}
      if(((string *)this->ptr)->size() == 0)//if string is empty, just return an empty string
	return *this;//TODO dont know if there is a faster way to do this, or if neccisary 
      ((string *)this->ptr)->erase(0,1);
      return *this;
    }else if(this->type==VECTOR){//remove first element
      ((vector<__ANY__> *)this->ptr)->erase(((vector<__ANY__> *)this->ptr)->begin());
      return *this;
    }else{
      return (*this)=(*this)-1;
    }
  }
  __ANY__ operator^(const __ANY__& a)const{//TODO
    __ANY__ b;
    //if a!=num throw
#ifndef TWINE__WIERD_ASSINGMENTS
    if(a.type==STRING||a.type==VECTOR||type==STRING||type==VECTOR){
      throw invalid_argument("Invalid assignment, using ^ on types: "+this->getType()+" and type: "+a.getType());
    }
#endif //TWINE_WIERD_ASSINGMENTS
    if(this->type == INT){
      b.type = INT;
      b.i = pow(this->i,a.toNum());
    }else if(this->type == DOUBLE){
      b.type = DOUBLE;
      //b.i = pow(*(double *)this->ptr,a.toNum());
      b.i = pow(this->d, a.toNum());
    }else{
      //TODO
      b = *this;
    }
    return b;
  }
  __ANY__ operator[](const __ANY__& a)const{//TODO make return reference
    __ANY__ b;
#ifndef TWINE__WIERD_ASSINGMENTS
    if(!(a.type == INT||a.type==DOUBLE||a.type==BOOL)){
      throw invalid_argument("Invalid assignment, [...] into invalid type: "+a.getType());
    }
#endif //TWINE_VERY_WIERD_ASSINGMENTS
#ifdef TWINE_SAFE
    if(this->length() <= a.toNum())//TODO doenst work for ints/doubles
      throw out_of_range(string("Out of obunds exception: [] at position ")+to_string((int) a.toNum())+" into type "+this->getType()+" with length "+to_string(this->length()));
#endif//TWINE_SAFE    
    if(this->type == STRING){
      b.type=STRING;
      char tmp = ((string*)this->ptr)->at(a.toNum());
      b.ptr = new string(1,tmp);//string of 1 char
    }else if(this->type == VECTOR){
      b = __ANY__(((vector<__ANY__>*)this->ptr)->at(a.toNum()));
    }else if(this->type == INT){
      b.type = INT;
      //TODO fix this
      string tmp = to_string((long long)this->i);
      char tmp2 = tmp.at((int)a.toNum());
      string tmp3 = string(&tmp2);
      b.i = stoi(tmp3);
    }else if(this->type == DOUBLE){
      b.type = INT;
      string tmp = to_string((long double)this->i);
      char tmp2 = tmp.at((int)a.toNum());
      string tmp3 = string(&tmp2);
      b.i = stoi(tmp3);
    }else if(this->type == BOOL){
      if(a.toNum() == 0) return __ANY__(*this);//return bool is looking at indecy 0, otherwise false
      else return __ANY__(false);
    }else{
      //TODO
    }
    return b;
  }

  bool operator!()const{
    return !(this->toNum());
  }
  bool operator>(const __ANY__& a)const{
    if(this->type == STRING && a.type == STRING){
      return (*(string*)a.ptr)>(*(string*)this->ptr);//this seems backwards but i think it works
    }else if((this->type == INT||this->type == BOOL||this->type == DOUBLE)&&(a.type == INT||a.type == BOOL||a.type == DOUBLE)){
      return this->toNum() > a.toNum();
    }else if(a.type == VECTOR && this->type == VECTOR){
      for(int i = 0; i < min(((vector<__ANY__> *)a.ptr)->size(),((vector<__ANY__> *)this->ptr)->size());i++){if ((*this)[i] < a[i]) return false;}return true;}
    return this->toNum() > a.toNum();
  }
  bool operator<(const __ANY__& a)const{//maks sure to do strings for alphabetical order
    if(this->type == STRING && a.type == STRING){
      return (*(string*)a.ptr)<(*(string*)this->ptr);//this seems backwards but i think it works
    }else if((this->type == INT||this->type == BOOL||this->type == DOUBLE)&&(a.type == INT||a.type == BOOL||a.type == DOUBLE)){
      return this->toNum() < a.toNum();
    }else if(a.type == VECTOR && this->type == VECTOR){
      for(int i = 0; i < min(((vector<__ANY__> *)a.ptr)->size(),((vector<__ANY__> *)this->ptr)->size());i++){if ((*this)[i] > a[i]) return false;}return true;}
    return this->toNum() < a.toNum(); 
  }
  bool operator>=(const __ANY__& a)const {return ((*this)>a || (*this) == a);}//TODO would check == first be better?
  bool operator<=(const __ANY__& a)const {return ((*this)<a || (*this) == a);}
  bool operator!=(const __ANY__& a)const {return !((*this) == a);}
  template<typename T> bool operator!=(const T& a)const {
    if(this->type != typeid(a).name()){
      return (*(T*) this->ptr) == a;
    }
#ifndef TWINE_WIERD_TYPE_CHECKS
    throw(string("Invalid check(!=)") + typeid(a).name() + " and __ANY__::"+this->getType());
#endif
    return false;//should this give a warning/error?
  }
  bool operator==(const __ANY__& a)const {
    if((this->type==DOUBLE || this->type == INT || this->type == BOOL) && (a.type == INT || a.type == DOUBLE || a.type == BOOL)){
      return this->toNum() == a.toNum();
    }
#ifndef TWINE_WIERD_TYPE_CHECKS
    if(this->type!=a.type){//if !TWINE_WIERD_TYPE_CHECKS
      return false;
    }
    if(a.type == STRING){
      return (*(string*)this->ptr== *(string*)a.ptr);
    }else if(a.type == VECTOR){//TODO
      if((*(vector<__ANY__>*)this->ptr).size() != (*(vector<__ANY__>*)a.ptr).size()){
	return false;
      }else{
	for(int i = 0; i < (*(vector<__ANY__>*)this->ptr).size();i++){//TODO use itterators?
	  if(!((*(vector<__ANY__>*)this->ptr)[i] == (*(vector<__ANY__>*)a.ptr)[i]))
	    return false;
	}
      }
      return true;
    }
#endif //TWINE_WIERD_TYPE_CHECKS
#ifdef TWINE_WIERD_TYPE_CHECKS//basically else
    if(type == a.type && type == STRING)
      return (*(string*)this->ptr== *(string*)a.ptr);
    if(type == a.type && a.type == VECTOR){//TODO
      if((*(vector<__ANY__>*)this->ptr).size() != (*(vector<__ANY__>*)a.ptr).size()){
        return false;
      }else{
        for(int i = 0; i < (*(vector<__ANY__>*)this->ptr).size();i++){//TODO use itterators?
          if(!((*(vector<__ANY__>*)this->ptr)[i] == (*(vector<__ANY__>*)a.ptr)[i]))
            return false;
        }
      }
      //TODO: this is where the wierd stuff goes, like checking if a string is true(not null) or something?
      //casts should be made so left side(this) is turned into the type of right, then checked
      if(type == STRING && a.type == INT)
	return !((*(string*)this->ptr).length() ^ a.i);
    }
#endif //TWINE_WIERD_TYPE_CHECKS
    return false;
  }
  template<typename T> bool operator==(const T& a)const{
    if(this->type == typeid(a).name()){//check type
      return (*(T*) this->ptr) == a;
    }
#ifndef TWINE_WIERD_TYPE_CHECKS
    throw invalid_argument(string("Invalid check(!=) between type: ")+typeid(a).name() + " and __ANY__::"+this->type);
#endif
    return false;
    }
  vector<__ANY__> toVector();//TODO

  unsigned char * toBytes(){//TODO or just bytes()
    //unsigned char * Bytes;
    if(type == INT){
      unsigned char * Bytes = (unsigned char*) malloc(sizeof(int) + 1);
      //unsigned char Bytes[sizeof(int) + 1];
      for (int i = 0; i < 4; i++)
	Bytes[3-i] = i >> (i * 8);
      Bytes[4] = '\0';
      return Bytes;
    }else if(type == STRING){
      unsigned char * Bytes = (unsigned char*) malloc(sizeof(char) * ((string *)ptr)->size()+1);
      //unsigned char Bytes[sizeof(char) * ((string *)ptr)->size()+1];
      //strcpy(Bytes, ((string *)ptr)->c_str());//TODO
      return Bytes;
    }
#ifndef TWINE_QUIET
    throw invalid_argument(string("Not implimented: toBytes() on type: ")+this->getType());
#endif //TWINE_QUIET
  }
  unsigned dataSize()const{
    if(type == INT)
      return 4;//size of int32_t
    if(type == DOUBLE)
      return 8;//sizeof double
    if(type == BOOL)
      return sizeof (bool);
    if(type == STRING)
      //return sizeof (*(string*) ptr);//or string.size() * sizeof char?
      return (sizeof(char)) * (*(string*) ptr).size();
    if(type == VECTOR){
      //return sizeof (*(vector<__ANY__>*) prt);//or vecotr.size()*sizeof char or loop through each and sum
      int total = 0;
      for(int i = 0;i<(*(vector<__ANY__>*) ptr).size();i++)
	total+=((*(vector<__ANY__>*)this->ptr))[i].dataSize();
      return total;
    }
#ifndef TWINE_QUIET
    throw invalid_argument("Can't find data size on unknown type: "+this->getType());
#endif //TWINE_QUIET
    return -1;
  }


  //~__ANY__(void){}
  
  friend ostream& operator<<(ostream& os, const __ANY__& a);
  
};

ostream& operator<<(ostream& os, const __ANY__& a){
  os << a.toString();
  return os;

}

const string __ANY__::STRING = "_S";
const string __ANY__::INT = "_I";
const string __ANY__::BOOL = "_B";
const string __ANY__::DOUBLE = "_D";
const string __ANY__::VECTOR = "_V";

//const string __ANY__::STRING = typeid(string()).name();
//const string __ANY__::INT = typeid(128256).name();
//const string __ANY__::BOOL = typeid(true).name();
//const string __ANY__::DOUBLE = typeid(183.249).name();
//const string __ANY__::VECTOR = typeid(vector<__ANY__>).name();

/*
const char __ANY__::STRING[] = a.c_str();//{'_', 'S', '\0'};
const char __ANY__::BOOL[] = {'_', 'B', '\0'};
const char __ANY__::INT[] = {'_', 'I', '\0'};
const char __ANY__::DOUBLE[] = {'_', 'D', '\0'};
const char __ANY__::VECTOR[] = {'_', 'V', '\0'};
*/

/*
#ifdef TWINE_WIERD_ASSIGNMENTS
__ANY__::__ANY__(const vector<string>& lst){
  type = VECTOR;
  vector<__ANY__> * INA= new vector<__ANY__>();
  for(int i = 0;i<lst.size();i++)
    INA->push_back(lst[i]);
  prt = INA;
}

__ANY__::__ANY__(const vector<double>& lst){
  type = VECTOR;
  vector<__ANY__> * INA= new vector<__ANY__>();
  for(int i = 0;i<lst.size();i++)
    INA->push_back(lst[i]);
  prt = INA;
}

__ANY__::__ANY__(const vector<int>& lst){
  type = VECTOR;
  vector<__ANY__> * INA= new vector<__ANY__>();
  for(int i = 0;i<lst.size();i++)
    INA->push_back(lst[i]);
  prt = INA;
}

__ANY__::__ANY__(const vector<bool>& lst){
  type = VECTOR;
  vector<__ANY__> * INA= new vector<__ANY__>();
  for(int i = 0;i<lst.size();i++)
    INA->push_back(lst[i]);
  prt = INA;
}
#endif //TWINE_WIERD_ASSIGNMENTS
*/

#endif //__ANY__H
