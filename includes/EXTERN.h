
#ifndef EXTERN_H
#define EXTERN_H

//#include <chrono>
#include <thread>//for threads
#include <atomic>// used for atomic static member vars

//#include <execinfo.h> //for printing stack trace
#include <unwind.h>
//#include <stdio.h>

#include <string>//String
#include <iostream>//cin, cout, endl
#include <fstream>//read, write file IO
//#include <ctime>
#include <vector>//lists
//#include <stdexcept>
#include <math.h>//pow, PI
#include <algorithm>//sort
#include <cstdarg>//Variadic functions

#include <array>//std::array
//#include <cstdlib>
//#include <initializer_list>
//#include <typeinfo>
#ifndef unix
#undef TWINE_SEGFUALT_PROTECTION
#endif

#ifdef TWINE_SEGFUALT_PROTECTION
#include <cstring>
#include <signal.h>
#endif //TWINE_SEGFUALT_PROTECTION


using namespace std;

#endif//EXTERN_H
