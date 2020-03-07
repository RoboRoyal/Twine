

#ifndef TWINELIB_TYPEINFO
#define TWINELIB_TYPEINFO

#include <string>
#include <typeinfo> //typeid

#include "__ANY__.h"
#include "Channel.h"

//look into std::type_info

string type(const int c){return "int";}
string type(const string& c){return "string";}
string type(const double c){return "double";}
string type(const bool c){return "bool";}
string type(const char c){return "char";}
string type(const __ANY__& c){return string("__ANY__::") + c.getType();}
template <typename T> string type(const vector<T> c){
  if(c.size())
    return string("Vector<")+type(c[0])+">";
  return string("Vector<")+typeid(T).name()+">";//it would probably be better to be consistent
}
template <class myType>
string type(const myType& a) {
  return typeid(a).name();
}

//if there ever was a standard, any new class should impliment string type(const T&) of its onw type

//template<class T>
//string type(const Channel<T>& c){return string("Channel::"+type(T));}

//TODO have this autogenerate for all classes user makes


unsigned long sizeOf(const __ANY__& a){
  return a.dataSize() + sizeof(a);//could replace with constant
}

template <class myType>
inline unsigned long sizeOf(const myType& a){// impliments as sizeof keyword
  return sizeof(a);//could this be done with #define sizeof( sizeOf( ?
}
//#define sizeOf( sizeof(


#endif //TWINELIB_TYPEINFO
