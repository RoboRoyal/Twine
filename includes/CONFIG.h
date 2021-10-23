
#ifndef _CONFIG_H_
#define _CONFIG_H_
/*
  TWINE_WEIRD_CONVERSIONS //bool to string
  TWINE_WIERD_ASSIGNMENTS //add bool to ints
  TWINE_VERY_WIERD_ASSINGMENTS //[] into ints, take the power of strings
  TWINE_QUITE //if defined, Twine wont throwe error messages. if defined, error are silent; undefined by defualt
  TWINE_FORCE //prog does everything it can to make whatever code is given work, or at least not crash or throw errors
  TWINE_DEBUG //prints some debug stuff
  TWINE_WIERD_TYPE_CHECKS //if defined, allows wierd type checks like int == string
*/

#define TWINE_SEGFUALT_PROTECTION


#ifdef TWINE_TYPE_SAFE//TWINE_STRICKED?
#undef TWINE_WIERD_ASSIGNMENTS
#undef TWINE_VERY_WIERD_ASSINGMENTS
#undef TWINE_WEIRD_CONVERSIONS
#undef TWINE_WIERD_TYPE_CHECKS
#undef TWINE_QUITE
#define TWINE_SEGFUALT_PROTECTION
#endif//TWINE_TYPE_SAFE

#ifdef TWINE_VERY_WIERD_ASSINGMENTS
#define TWINE_WIERD_ASSIGNMENTS
#endif //TWINE_VERY_WIERD_ASSINGMENTS

#ifdef TWINE_FORCE
#define TWINE_WEIRD_CONVERSIONS
#define TWINE_WIERD_ASSIGNMENTS
#define TWINE_VERY_WIERD_ASSINGMENTS
#define TWINE_WIERD_TYPE_CHECKS
#define TWINE_QUITE
#define TWINE_SEGFUALT_PROTECTION
#endif //TWINE_FORCE

/**********TWINE GLOBALS***********************/
//TIME
#define EXECUTION_TIME ((double)TWINE_CLOCK)/CLOCKS_PER_SEC
#define CURRENT_TIME_MS (double)chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count()
#define RUNNING_TIME (CURRENT_TIME_MS-OTHER_START_TIME)/1000

//MATH
#define PI M_PI
//const double PI = 3.14159265358979323846;
//const double PI = M_PI;


#endif//_CONFIG_H_
