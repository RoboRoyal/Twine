
#ifndef EXTERN_H
#define EXTERN_H

//#include <chrono>
#include <thread>//for threads
#include <atomic>// used for atomic static member vars

//#include <execinfo.h> //for printing stack trace
#ifndef v_studio_MSVC
#include <unwind.h>
#endif //v_studio_MSVC


#include <algorithm>//sort
#include <array>//std::array
#include <cstdarg>//Variadic functions
#include <fstream>//read, write file IO
#include <iostream>//cin, cout, endl
#include <math.h>//pow, PI
#include <string>//String
#include <vector>//lists


//#include <ctime>
//#include <stdexcept>
//#include <stdio.h>
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
