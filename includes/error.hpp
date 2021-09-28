
#ifndef _ERROR_H_
#define _ERROR_H_

#include <cxxabi.h>


void __finish__();
const int maxStackTraceSize = 24 + 6;

string demangleName(const string& mangled){
   const char * name= mangled.data();
   int status = 0;
   size_t funcnamesize = 1024;
   char* funcname = (char *)malloc(sizeof(char) * funcnamesize);
   char* ret = abi::__cxa_demangle( name, funcname,&funcnamesize, &status );

   if(*funcname){
     string tmp = funcname;
     free(funcname);
     return tmp;
   }
   free(funcname);
   return mangled;
 }

#if defined(unix)
vector<string> getStack(){
  vector<string> ret = vector<string>();
  void *array[maxStackTraceSize];
  size_t arrSize;
  arrSize = backtrace(array, maxStackTraceSize);

  char ** stackString = backtrace_symbols(array, arrSize);
  for(unsigned i = 1; i < arrSize - 1; i++){
    string me = string(stackString[i]);
    int st = me.find_first_of("(")+1;
    me = me.substr(st, me.length()-st);
    int ft = me.find_first_of("+");
    me = me.substr(0, ft);
    ret.push_back(demangleName(me));
  }
  free(stackString);
  return ret;
}
#else
vector<string> getStack() {
  vector<string> ret = vector<string>();
  ret.push_back("Stack trace only works on linux for now");
  return ret;
}
#endif

#if defined(unix)
void printStack(const unsigned deBuffSize = 1, const unsigned endBuffSize = 3) {
  void *array[maxStackTraceSize];
  size_t arrSize;

  arrSize = backtrace(array, maxStackTraceSize);

  char ** stackString = backtrace_symbols(array, arrSize);
  string file;
  unsigned fileStringLen = 0;
  for(bool found = false; !found; fileStringLen++){
    if(stackString[4][fileStringLen] == '('){
      found = true;
    }else{
      file+=stackString[4][fileStringLen];
    }
  }
  cout<<"In file "<<file<<endl;
  for(unsigned i = deBuffSize; i < arrSize - endBuffSize; i++){
    string me = string(stackString[i]);
    const string origonal = me;
    cout<<'['<<(arrSize-i-1-endBuffSize)<<"] ";
    int st = me.find_first_of("(")+1;
    me = me.substr(st, me.length()-st);
    int ft = me.find_first_of("+");
    me = me.substr(0, ft);
    if(me != ""){
      cout<<demangleName(me)<<endl;
    }else{
      cout<<origonal<<endl;
    }
  }
  free(stackString);

}
#else
void printStack(const unsigned deBuffSize = 1, const unsigned endBuffSize = 3) {
  cout << getStack()[0] << endl;
}

#endif

string getErrorType(const int signal){
  switch(signal){
  case(1): return "Hangup";
  case(2): return "Keyboad Interupt";
  case(3): return "Keyboard Kill Command";
  case(6): return "Aborted";
  case(9): return "Kill Signal";
  case(11): return "Segmentation Fault";
  case(12): return "Bad System Call";
  case(15): return "Program Terminated";
  case(16): return "User Killed";
  }
  return "";
}


void handler(const int sig){
  
  cout<<"Error in program "<<sig;
  string tmp = getErrorType(sig);
  if(tmp != "")
    cout<<"("+tmp+")";
  cout<<endl;
  printStack(3);
  __finish__();
  exit(sig);
}
#if defined(unix)
void segFaultHandler(int signal, siginfo_t *si, void *arg){
  
  cout<<"Error in program "<<signal;
  string tmp = getErrorType(signal);
  if(tmp != "")
    cout<<"("+tmp+")";
  //cout<<" at address "<<si->si_addr<<endl;
  printf(" at address: 0x%lx\n",(long) si->si_addr);
  printStack(3);
  __finish__();
  exit(signal);
}
#else
/*void segfaultHandler(int signal, void * si, void *arg){
    cout<<"Segfualt cuased abort; signal "<<signal<<endl;
    printStack(0,1);
    exit(signal);
}*/

#endif

#endif //_ERROR_H_
