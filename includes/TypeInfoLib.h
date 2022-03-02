

#ifndef TWINELIB_TYPEINFO
#define TWINELIB_TYPEINFO

#include <string>
#include <typeinfo> //typeid

#include "__ANY__.h"
//#include "Channel.h"

//look into std::type_info

string type(const int c){return "int";}
string type(const string& c){return "string";}
string type(const double c){return "double";}
string type(const bool c){return "bool";}
string type(const char c){return "char";}
string type(const __ANY__& c){return string("__ANY__::") + c.getType();}
template <class myType>
string type(const myType& a) {
  return typeid(a).name();
}
template <typename T> string type(const vector<T>& c){
  if(not c.empty())
    return string("Vector<")+type(c[0])+">";
  return string("Vector<")+typeid(T).name()+">";//it would probably be better to be consistent
}

template <typename T> string type(const T c[]){
   if(sizeof(c))//is this needed?
     return string("Array<")+type(c[0])+">";
  return string("Array<")+typeid(T).name()+">";
}

template <typename T, size_t N> string type(const array<T,N>& c){
  if(not c.empty())
    return string("Array<")+type(c[0])+","+to_string(N)+">";
  return string("Array<")+typeid(T).name()+">";
}

//if there ever was a standard, any new class should implement string type(const T&) of its onw type

//template<class T>
//string type(const Channel<T>& c){return string("Channel::"+type(T));}

//TODO have this autogenerate for all classes user makes


unsigned long sizeOf(const __ANY__& a){
  return a.dataSize() + sizeof(a);//could replace with constant
}

template <class myType>
inline unsigned long sizeOf(const myType& a){// implements as sizeof keyword
  return sizeof(a);//could this be done with #define sizeof( sizeOf( ?
}
//#define sizeOf( sizeof(


#endif //TWINELIB_TYPEINFO
