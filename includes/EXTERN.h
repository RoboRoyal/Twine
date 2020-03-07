
#ifndef EXTERN_H
#define EXTERN_H

//#include <chrono>
#include <thread>//for threads
#include <atomic>// used for atomic static member vars

#include <execinfo.h> //for printing stack trace
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

#ifdef TWINE_SEGFUALT_PROTECTION
#include <cstring>
#include <signal.h>
#endif //TWINE_SEGFUALT_PROTECTION

using namespace std;

#endif//EXTERN_H
