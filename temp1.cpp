
/*
**Twine Header, template file 1
*/

/*
#define TWINE_SAFE
#include "includes/CONFIG.h"
#include "includes/EXTERN.h"
#define int int32_t
#include "includes/__ANY__.h"
#include "includes/TwineLib.h"
*/

void __initiateStaticVars__();
void __finish__();
int __userMain__(__ANY__& args);
bool __unjoinedThreads__();

using namespace std;
/************Global Vars***********/
clock_t TWINE_CLOCK;
chrono::system_clock::time_point PROGRAM_START_TIME;
double OTHER_START_TIME;
//const unsigned maxStackTraceSize = 24 + 6;
/*********End Global Vars***********/

//#include "includes/error.h"

void __INIT__(){
  srand(time(NULL) + randInt());//set up random
  TWINE_CLOCK = clock();//track execution time
  PROGRAM_START_TIME = chrono::system_clock::now();//stored as crono
  OTHER_START_TIME = CURRENT_TIME_MS;//stored as double
#ifdef TWINE_SEGFUALT_PROTECTION
  signal(SIGSEGV, handler);
  signal(SIGABRT, handler);
  signal(SIGINT, handler);
  signal(SIGTERM, handler);
  struct sigaction sa;
  memset(&sa, 0, sizeof(struct sigaction));
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = segFaultHandler;
  sa.sa_flags   = SA_SIGINFO; 
  sigaction(SIGSEGV, &sa, NULL);
#endif//TWINE_SEGFUALT_PROTECTION
  //__initiateStaticVars__();//set up initial values of all static vars
}
void __finish__(){
  if(__unjoinedThreads__()) cout<<"orphan threads found :("<<endl;
  //free memory?
}
int main(int argc, char *argv[]){
  __INIT__();
  vector<__ANY__> * params = new vector<__ANY__>;//convert passed in parameters to vector
  for(int i = 0;i<argc;i++){
    __ANY__ N =  __ANY__();
    N.ptr = new string(argv[i]);
    N.type = "_S";
    params->push_back(N);
  }
  __ANY__ p;
  p.type = "_V";
  p.ptr = params;
  int returnStat = -1;
  try{
    returnStat = __userMain__(p);
    __finish__();
  }catch(const baseException& e){
#ifndef TWINE_QUIET
    cout<<e.what()<<endl;
#endif //TWINE_QUIET
    returnStat = -2;
  }catch(const exception& e){
#ifndef TWINE_QUIET
    cout<<"FAILED: "<<e.what()<<endl;
#endif //TWINE_QUIET
    returnStat = -3;
  }
  //delete params;
  return  returnStat;
}
#define MAX_T 8
//user function decloraions here
