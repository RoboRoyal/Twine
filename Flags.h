 /*
 * Flags.h
 *
 *  Created on: Dec 1, 2018
 *      Author: Dakota
 * Includes most of the info/options used for running the program
 */

#ifndef FLAGS_H
#define FLAGS_H

#include "utils/Twine_Utils.h"

using namespace std;

//Global Flags
string TWINE_VERSION = "0.1";
unsigned TWINE_VERSION_INT = 2;//is the exit stauts when asking for integer version
string INSTALL_PATH = "/etc/twine/src/";
int reportingLevel = 2;//what data should be reported to user: -2 = debug, 0=info, 2 = warnings, 4 = errors
bool FORCE = false;//attempt fo fix mistakes and force to run
bool interpMode = false;//running in interpretive mode, rather than compiled

vector<string> inFiles = vector<string>();//list of files to read in
string cppFileName;
string pathToFile = "";//used to get includes to file
string mainFileName;
string executableFileName;

string pathToDownloadedImports = "downloadedImport/";

//Lexer Flags --probably should move into a list to be accessed by getFlag
bool SAVE_LINE_COMMENTS = false;
bool SAVE_BLOCK_COMMENTS = false;
bool SAVE_WHITESPACE = false;
bool SCRUB = false;

//Driver Flags
bool parse = true;
bool compileProg = true;
bool executeProg = true;
bool lint = false;
bool format = false;
bool removeCPPFileAfter = true;
bool usingPreCompiledHeaders = true;

//C++ compile flags
bool usingClang = false;
//enum Optimizations{O0,O1,OS,O2,O3,Of};

string O_OUT_FILE = "";

const string warningIgnore = "-Wnarrowing -Wno-return-type";

string compileOptions = "";
const string specialOptions = "-fmerge-all-constants -g -rdynamic -fno-signed-zeros -fno-keep-static-consts -pthread -march=native -funsafe-math-optimizations -fpermissive";//fun and safe math? sign me up!
const string specialOptions_clang = "-fmerge-all-constants -rdynamic -fno-signed-zeros -funsafe-math-optimizations -march=native -pthread";
const string ignor_warning = "-w";
const string ignor_warning_clang = "-Weverything";
string runParams = "";

//Twine compiler flags
struct flagOptions{
  string name;
  bool * enabled;
  string help;
  //string enableFlag;
  //string disableFlag;
  string toString() const{
    return help+" ; Is enabled? "+((*enabled)?"True":"False");
  }
};

struct Flag{
  string name;
  bool enabled;
  string help;
  string toString() const{
    string ret = name +": ";
    for(int i = 0; i<30-name.length();i++) ret+=" ";
    ret+=help;
    for(int i = 0; i<112-help.length();i++) ret+=" ";
    return ret+"Enabled: "+(enabled?"True":"False");
  }

};
Flag compileFlags[] = {
		       //effects durring C++ compiling, used in Trans
		       {"FORCE",false,"Attempts to fix mistakes and force prgram to run(Also enables other flags)"},
		       {"TWINE_WIERD_ASSIGNMENTS", true,"Allows wierd assignments to be done such as adding bools to ints"},
		       {"TWINE_WIERD_CONVERTIONS", true,"Allows wierd convertions of type, such as converting bool to string"},
		       {"TWINE_VERY_WIERD_ASSINGMENTS", false,"Allows wierd operations, such as [] into ints"},
		       {"TWINE_QUITE", false, "If enabled, Twine wont throw errors(errors will be silent)"},
		       {"TWINE_DEBUG", false, "Prints some debug info to the sceen"},
		       {"TWINE_WIERD_TYPE_CHECKS", false,"Allows for checks between diffrent type of objects, such as int == string"},
		       {"TWINE_TYPE_SAFE", false,"Enables stricked type checking(disables WIERD_CONVERTIONS, WIERD_ASSINGMENTS, TWINE_QUITE, WIERD_TYPE_CHECKS"},
		       {"SAVE_LINE_COMMENTS",false,"Saves single line comments from origonal file and puts them into the output(C++) file"},//TODO impliment these two
		       {"SAVE_BLOCK_COMMENTS",false,"Saves block comments from origonal file and puts them into the output(C++) file"},
		       
		       //should be these in a different list?
		       //these are used internal to the lex-parse-trans process
		       /*Parser*/
		       {"DEFUALT_RETURN", true, "Adds a falsey/0 defualt return value to functions with a return type but no explicit return statement"},
		       {"CLASS_REDEFINITIONS", false,"Allows redefinitions of classes"},//redef or addition?
		       {"FUNCTION_REDEFINITION", true,"Allows redefinition of fucntions"},
		       {"WARNINGS_ARE_ERRORS", false, "Turns all warnings into errors"},
		       {"EXIT_ON_ERROR", false, "Exits parsing on first error"},
		       {"TRY_TRANS", false, "Try to output a .cpp file even if there were some error"},
		       {"TRY_COMPILE", false, "Try to compile in C++ even if there were some error"}, //not implimented
		       {"__TWINE__", true, "Should always be enabled. Used to signal to included files that they are being used in a twine program"},
};
Flag parserFlags[] = {
		      {"DEFUALT_RETURN", true, "Adds a falsey/0 defualt return value to functions with a return type but no explicit return statement"},
		      {"LOCAL_VAR_REDEFINITION", false, "Allows for the redefinition of local vars. (ex. int x=0 string x=\"str\")"},
		      {"VAR_REDEFINITION", true, "Allows redefinitions of vars from higher scopes. (ex. int x=0 {string x=\"str\"})"},
		      {"AUTO_FUNCTIONS", true, "Adds in defualt functions for classes"},
		      {"AUTO_TO_STRING", true, "Adds in a toString function for classes without one already"},
		      {"AUTO_TO_STRING_STATIC", false, "Adds a static toString function for classes without one already"},

		      //warnings
		      {"IMPLICIT_CASTS_ON_OBJECTS", true, "Throws a wrning if you implicitly cast an object to a number or string"},
};

Flag formaterFlags[] = {
			{"FIX_INDENTATION", true, "Ensures indentation in accordnce with { }"},//
			{"FIX_BLOCK_NEWLINES", true, "Ensures needed new lines after {"},//
			{"FIX_CLOSING_BLOCK", true, "Ensures a new line after }, except if there is an else/elif"},
			{"FIX_DOULBE_CLOSING", true, "Ensures new line between two closing braces '}'"},//
			{"ONE_LINE_AT_A_TIME", true, "Ensures one parsable line per line"},
			{"USE_SPACES", true, "Uses two spaces instead of tabs for indentation"},//
			
			{"REMOVE_EXTRA_LINES", false, "Removes excess lines(more than 3 in a row)"},
			
			
};
Flag driverFlags[] = {
		      {"removeCPPFileAfter", true, "Removes the auto-generated cpp file after file execution"},
		      {"removeOFileAfter", true, "Removes the auto-generated o file after file execution"},
		      
};

//get compiler or parser flag
bool getFlag(string flagName){
  for(short i = 0; i != sizeof compileFlags / sizeof compileFlags[0];i++)
    if(flagName == compileFlags[i].name)
      return compileFlags[i].enabled;
  for(short i = 0; i != sizeof parserFlags / sizeof parserFlags[0];i++)
    if(flagName == parserFlags[i].name)
      return parserFlags[i].enabled;
  for(short i = 0; i != sizeof formaterFlags / sizeof formaterFlags[0];i++)
    if(flagName == formaterFlags[i].name)
      return formaterFlags[i].enabled;
  cout<<"ERROR! checking for flag that doesnt exist: "<<flagName<<endl;
  throw runtime_error("checking for flag that doesnt exist");
}

bool setFlag(string flagName, int set){
  flagName = toUpper(flagName);
  for(short i = 0; i != sizeof compileFlags / sizeof compileFlags[0];i++)
    if(flagName == compileFlags[i].name){
      compileFlags[i].enabled = set;
      return true;
    }
  for(short i = 0; i != sizeof parserFlags / sizeof parserFlags[0];i++)
    if(flagName == parserFlags[i].name){
      parserFlags[i].enabled = set;
      return true;
    }
  for(short i = 0; i != sizeof formaterFlags / sizeof formaterFlags[0];i++)
    if(flagName == formaterFlags[i].name){
      formaterFlags[i].enabled = set;
      return true;
    }
  return false;
}

bool resolveFlag(string flag){
  if(strEql(flag.substr(0, 3), "fno")){
    return setFlag(toUpper(flag.substr(3, flag.size() - 3)), false);
  }
  if(strEql(flag.substr(0, 1), "f")){
    return setFlag(toUpper(flag.substr(1, flag.size() - 1)), true);
  }
  return false;
}
struct lintFlag{
  string name;
  bool enabled;
  int number;
  string help;
  string toString() const{
    string ret = name +": \t"+(name.length()>21?"":"\t")+help;
    for(int i = 0; i<88-help.length();i++) ret+=" ";
    return ret+"Enabled: "+(enabled?"True":"False")+"\tValue is sent to: "+to_string(number);
  }
};
  
lintFlag linterFlags[] = {//alternitivly, dont need a bool, just set int to 0 if disabled. Or could use bool as warning/error enable
			  //TODO make the names better-more consistant
			  {"VAR_MAX_LENGTH", true, 20, "Maximum length of varriables allowed"},//
			  {"VAR_MIN_LENGTH", true, 3, "Minimum length of varriables allowed"},//
			  {"PROPER_VAR_NAMES", true, 3, "Checks if name is in proper format (1:a_b vs 2:a_B vs 3:aB)"},//
			  {"VAR_TYPE_DEFINED", false, 1, "Makes all vars must have type"},//
			  {"CAPITALISE_CONSTANTS", true, 1, "Checks if all constants are in all caps"},//
			  {"LINE_MAX_LENGTH", true, 90, "Maximum length of a single line"},//
			  {"EXCESS_PARENTHESES", true, 1, "Checks for un-neccisarry usage of ( )"},//
			  {"IMPLICIT_CONVERTIONS", true, 1, "Checks for implicit convertions, such as int x = string"},//?
			  {"NUM_TO_STRING", true, 1, "Checks for convertions from numbers(int/double/bools) to strings (string s = 123)"},
			  {"STRING_TO_NUM", true, 1, "Checks for convertions from string to numbers(int/double/bools)"},
			  {"FUNCTION_NAME_MAX_LENGTH", true, 30, "Maximum length of function names allowed"},//
			  {"FUNCTION_NAME_MIN_LENGTH", true, 3, "Minimum length of fuction names allowed"},//
			  {"FUNCTION_NAME_PROPER", true, 3, "Checks if the name of a function is in the proper format (1:a_b vs 2:a_B vs 3:aB)"},//
			  {"MISSING_RETURN_STATMENTS", false, 1, "Checks if functions without void return has a return statment"},//
			  {"FUNCTION_COMPLEXITY", true, 200, "Maximum complexity of a function"},//
			  {"EXCESS_NEW_LINE", true, 3, "Checks for excesive consecutive new lines"},//
			  {"EXPRESSION_MAX_COMPLEXITY", true, 20, "The max complexity of any single expression"},//
			  {"EXPRESSION_MAX_LENGTH", true, 30, "The max number of atoms+OPs a single exprtession can have"},//
			  {"NUMBER_OF_COMMENTS", true, 8, "How many lines of code you should have per comment"},//
			  {"NEW_LINE_FOR_BLOCK", true, 1, "Requires a new line at the start of a block({block})"},//
			  {"NO_SIMILAR_NAMES", false, 1, "Does not allow vars/functions/classes of similar names to avoid confution"},//
			  {"NO_EMPTY_BLOCKS", false, 1, "Checks for empty blocks"},//
			  {"SHOW_COMPLEXITIES", false, 1, "Shows breakdown of complexities of every function/class"},//
			  
			  {"NO_MISSING_RETURNS", true, 1, "Requires every non-void function to have a return statment as the last line"},
			  {"NEW_LINE_AFTER_BLOCK", true, 1, "Requires a new line after }"},
			  {"ONE_LINE_PER_LINE", false, 1, "Requires each parsable line to be on its own line"},
			  {"MAX_COMPLEX_BASE", true, 3, "Max complexity of a base of class call ie. (a+b+c).type()"},
			  {"MAX_SEQUENTIAL_CALLS", false, 3, "Number of class calles at one time ie. a.b.c()"},
			  {"BLOCK_INDENTATION", true, 1, "Match indentation level of blocks {}"},
			  
			  {"OBJECT_MAX_FUNCTIONS", false, 20, "Maximum number of functions allowed for one object(or class)"},//
			  {"OBJECT_MAX_VARS", false, 10, "Max number of member vars allowed per object"},//
			  {"OBJECT_NAME_PROPER", true, 1, "Checks if the object has correctly formatted name (1:a_b vs 2:a_B vs 3:aB)"},//
			  {"OBJECT_NAME_LENGTH_MIN", true, 3, "Minimum length of class allowed"},//
			  {"OBJECT_NAME_LENGTH_MAX", true, 20, "Minimum length of class allowed"},//
};

//enables all linter flags
void setAllLintFlags(){
  for(short i = 0; i != sizeof linterFlags / sizeof linterFlags[0];i++)
    linterFlags[i].enabled = true;
}
//if enabled, returns count, otherwise returns 0
int getLintFlag(string flagName){
  flagName = toUpper(flagName);
  for(short i = 0; i != sizeof linterFlags / sizeof linterFlags[0];i++)
    if(flagName == linterFlags[i].name)
      if(linterFlags[i].enabled){
	return linterFlags[i].number;
      }else{ return 0;};
  
  cout<<"ERROR! checking for linter flag that doesnt exist: "<<flagName<<endl;//This would mean there is a code issue
  throw runtime_error("checking for flag that doesnt exist");
}

//enables or sets value of linter flag
bool setLintFlag(string flagName, int num, bool enable){
  //TODO set flagName to upper
  for(short i = 0; i != sizeof linterFlags / sizeof linterFlags[0];i++){
    if(flagName == linterFlags[i].name){
      if(enable){
	linterFlags[i].enabled = true;
	if(num != -1)
	  linterFlags[i].number = num;
      }else{
	linterFlags[i].enabled = false;
      }
      return true;
    }
  }
  return false;//flag doesnt exist
}


bool setLintFlag(string flagName, bool enable = true){
  //TODO set flagName to upper
  for(short i = 0; i != sizeof linterFlags / sizeof linterFlags[0];i++){
    if(flagName == linterFlags[i].name){  
      linterFlags[i].enabled = enable;
      return true;
    }
  }
  return false;//flag doesnt exist
}

struct options{
  string name;
  string alias;
  string parameters;
  string help;
  string toString(){
    string ret = name;
    if(alias != "none")
      ret += " ("+alias+") ";
    if(parameters != "none")
      ret += " "+parameters;
    int len = ret.length();
    for(int i = 0; i<60-len;i++) ret+=" ";
    ret += help;
    return ret;
  }
};

options commandLineOptions[] {//TODO order, finish, etc..
			    //NAMNE, SHORT FLAG, ARGS, HELP
			    {"force", "none", "none", "Sets the force flag that tries to correct user errors and forces program to compile"},
			      {"help", "h", "string what", "Displays help info about Twine or anything passed to it"},
				{"interp", "none", "none", "Enters the interpreter"},
				  {"debug", "d", "none", "Sets the debugger level to -2"},//should be verbose -v?
				    {"reportingLevel", "rl", "int level", "Sets the repoting level. -2=debug level, 0=info, 2=warning, 4=errors"},
				      {"execute", "x", "bool execute=false", "Enable/disables execution of passed in file"},
					{"cppFileName", "c", "string file", "Output cpp file name"},
					  {"outFileName", "o", "string file", "Output compiled file name"},
					    {"lint", "l", "none", "Lints the file, checking for correct coding standards"},
					      {"clang", "none", "none", "Compiles using clang"},
						{"benchmark", "none", "none", "Times the lexing, parsing and transcompiling time"},
						  {"format", "none", "none", "Formats the input file"},
						    {"set", "s", "string flag, int/bool value", "Sets flag to value, or just enables it if no value is given"},
						      {"unset", "u", "string flag", "Sets flag to false"},
							{"args", "none", "* args", "Passes the rest of the arguments to the program"},
							  {"file", "f", "string fileName", "Adds file to be compiled"},
							    {"quiet", "q", "none", "Turns reporting leve to 5, shunting all messages, including errors"},
							      {"usingPreCompiledHeaders", "none", "bool useCompiledHeaders=true", "Makes the compiler use regular or compiled header files in output"},
							      };

#endif /*FLAGS_H*/
