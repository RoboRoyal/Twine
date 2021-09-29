
/*
Main file for handling command line inputs
Handls calls to lexer, parser, and trans
*/

/*
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/
#include "utils/Twine_Utils.h"
#include "Lexer.cpp"
#include "2Parser.cpp"
#include "Trans.cpp"
#include "Interp.cpp"
#include "Formater.cpp"
#include "resolver.cpp"
//#include "trial.cpp"
//#include "testBench.cpp"

//#include <execinfo.h> //stack trace
#include <unwind.h>
#include <cstring>//segfualt
#include <signal.h>//segfualt
#include <chrono>//high_resolution_clock
using namespace std::chrono;

string argsForProg = "";      //args that are passed on to the user executing porgram

bool benchmarkTime = false;   //time how long each process takes
bool doneOtherThings = false; //says if the user has done something other than compile a prog
bool start_interp = false;    //start interpreter after everything

void help(string what = "");                                           //prints help info
void about();                                                          //unused
void getFlags(int argc, char** argv);                                  //parses comamnd line args
void printFuncs();                                                     //prints the functions recieved from parsing
bool compile(const string& fileInName, const string& fileOutName);     //compiles a C++ program using system commands
bool execute(const string& outName);                                   //executes a given file
bool doTheThing();                                                     //calls lexer, parser, transCompiler, compiler, executer chain / linter / formater
void freeProg();                                                       //frees memory-TODO
#if defined(unix)
void segfaultHandler(int signal, siginfo_t *si, void *arg);
#endif
void handler(int sig);



int main(int argc, char** argv){
  //testThing();
  //exit(5);
  if(true){

    signal(SIGSEGV, handler);
    signal(SIGABRT, handler);
    signal(SIGINT, handler);
    signal(SIGTERM, handler);

#if defined(unix)
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = segfaultHandler;
    sa.sa_flags   = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
#endif
  }

  
  usingPreCompiledHeaders = false;//TODO for now....
#ifndef TWINE_INSTALL_BUILD
  //cout<<"Term: "<<getenv("TERM")<<endl;
  reportingLevel = -2;
  INSTALL_PATH = "";
  removeCPPFileAfter = false;
#endif

#ifdef TWINE_INSTALL_BUILD
  compileOptions += " -Wnarrowing ";
  reportingLevel = 2;
  usingPreCompiledHeaders = false;//TODO for now....
#endif
  
#ifdef TWINE_USING_CLANG
  usingClang = true;
#endif
  cout<<"here at lease"<<endl;
  if(argc == 1){
    setFlag("DEFUALT_RETURN", false);
    Interp();
    return 0;
  }
  getFlags(argc, argv);//parse command line args, they get tored in Flags and vecctor<string> inFiles, along with a few globals
  if(inFiles.size() == 0 && !doneOtherThings){//if no files or flags to do something set, go to interpreting mode
    setFlag("DEFUALT_RETURN", false);
    Interp();
    return 0;
  }
  if(inFiles.size() == 0 && doneOtherThings)
    return 0;
  if(cppFileName == ""){
    cppFileName = mainFileName + ".cpp";
  }
  if(executableFileName == ""){
    executableFileName = mainFileName + ".o";
  }
  try{
    if(!doTheThing())
      return 3;
  }catch(invalid_argument e){
    cout<<"Error found in compile/execution: "<<e.what()<<endl;
    return 4;
  }
  report("recursed: "+to_string(recused), -1);//imjustakid?
  return 0;
}

//Main handling code for reading->lexing->parsing->compiling->executing chain
bool doTheThing(){//TODO
  string data;
  high_resolution_clock::time_point start = high_resolution_clock::now();
  high_resolution_clock::time_point t1 = start;

  if(!connectFiles(&inFiles,&data))
    return false;
  if(benchmarkTime){duration<double> time_span = duration_cast<microseconds>( high_resolution_clock::now() - t1 );
    report("Time to read and connect files: "+to_string((long double) time_span.count()), 2);}
  vector<TokenData> tokens;
  t1 = high_resolution_clock::now();
  if(!Lexer(&data,&tokens)){
    report("Failed to lex file, terminating program", 4);
    return false;
  }
  if(benchmarkTime){duration<double> time_span = duration_cast<microseconds>( high_resolution_clock::now() - t1 );
    report("Time to lex: "+to_string((long double) time_span.count()), 2);}
  if(reportingLevel < 0){//print out tokenized data
    for(vector<TokenData>::iterator i = tokens.begin(); i != tokens.end(); i++){
      report(TokenData::getTokenTypeName(i->tokenType)+" '"+i->tokenText+"' at position(line:char) "+to_string((long long)i->line)+": "+to_string((long long)i->charPos),-1);
    }
    report("\n\n--------------------------\n\n",-1);
  }
  t1 = high_resolution_clock::now();//time to finish read/lex
  if(format){
    Format(&tokens);
    writeFile(cppFileName.c_str(),&out);
  }
  if(parse && (!ParseTwo(&tokens) && !getFlag("TRY_TRANS"))) return false;//parse tokens here
  if(parse) try{ cleanUp(); clearLexertokens(&tokens);}catch(exception e){report(string("Cleanup failed: ")+e.what(), -2);}//TODO fix cleanup
  if(benchmarkTime){duration<double> time_span = duration_cast<microseconds>( high_resolution_clock::now() - t1 );
    report("Time to parse(2): "+to_string((long double) time_span.count()), 2);}
  if(parse && reportingLevel < 0) printFuncs();
  t1 = high_resolution_clock::now();


  if(compileProg){
    if(!errors){
      if(!Trans())
	return false;
    }else{
      report("Could not run file, "+to_string((long long) errors) +" error"+(errors == 1?"":"s")+" found", 2);
      compileProg = false;
    }
  }
  if(benchmarkTime){duration<double> time_span = duration_cast<microseconds>( high_resolution_clock::now() - t1 );
    report("Time to trans: "+to_string((long double) time_span.count()), 2);}

  cout<<flush;
  duration<double> time_span = duration_cast<microseconds>( high_resolution_clock::now() - start );
  report("Total time: "+to_string((long double) time_span.count())+" seconds using "+to_string((long long) numberOfNodes)+ " nodes",0);
  if(!compileProg) executeProg = false;
  if(compileProg && !compile(cppFileName,executableFileName)){ 
    return false;
  }
  if(executeProg && !execute(executableFileName)){
    return false;
  }
  freeProg();
  return true;
}

void printFuncs(){
  cout<<"\n Have "<<Prog->functions.size()<<" funks"<<endl;
  for(vector<Funk *>::iterator i = Prog->functions.begin(); i != Prog->functions.end(); i++){
    cout<<(*i)->toString()<<endl;
    cout<<"\n---------------------------------------------\n"<<endl;
  }    
}
bool compile(const string& fileInName, const string& fileOutName){
  report("Compiling...",0);
  if(fileInName == fileOutName){
    report("Output file must have different name than input file",4);
    return false;
  }
  string Optimise = "-O0";//-O2 produces wrong output sometimes
  const string command = string((usingClang? "clang ":"g++ ")+fileInName+" -o "+fileOutName+" "+Optimise+" -std=c++17 "+compileOptions+" "+(usingClang? specialOptions_clang : specialOptions));
  report(command,0);
  if(int compileStatus = system(command.c_str())){
    report("failed to compile in C++: "+to_string((long long) compileStatus),4);
    return false;
  }else{
    report("Success compiling",0);
    if(removeCPPFileAfter)
      int doll = system((string("rm ")+fileInName).c_str());
    return true;
  }
}
bool execute(const string& name){
  report("Executing...",0);
  string command = "";
#if defined(unix)
  command = string("./");
#endif
  command += name + ' ' + argsForProg;
  int returnStatus;
  try{
    report(command,0);
    if(!(returnStatus = system(command.c_str()))){
      report("Executing successful! (exit code: "+to_string(returnStatus)+')',0);
      return true;
    }else{
#if defined(unix)
      report("ERROR while executing code: "+to_string(WEXITSTATUS(returnStatus)),4);
#else
        report("ERROR while executing code: IDK cuz we on windows", 4);
#endif
    }
  }catch(int e){
    report("ERROR, exception occured while try to execute code",4);
  }
  //remove .o file?
  //if(removeOFileAfter)
  //int doll = system((string("rm ")+name).c_str());
  return false;
}

bool stringIsTrue(const string& str){
  return (str == "true" || str == "t" || str == "1");
}
void getFlags(int argc, char** argv){
  for(int i = 1; i < argc; i++){
    string OGarg = string(argv[i]);
    string arg = toLower(string(argv[i]));
    if(arg[0] == '-'){
      string flag=arg.substr(1, string::npos);
      if(flag == "force"){
	report("setting force",-2);
	FORCE = true;
      }else if(flag == "h" || flag == "help"){
	++i;
	if(i==argc){help();  break;}
	help(argv[i]);
      }else if(flag == "install_path"){
	i++;
        if(i==argc) throw invalid_argument("INSTALL_PATH flag given without path given");
	INSTALL_PATH = argv[i];
      }else if(flag == "about"){
	about();
      }else if(flag == "interp"){//should just run when no parameters are passed?
	doneOtherThings = true;
	setFlag("DEFUALT_RETURN", false);
	Interp();
	//might need to re-add prog?
      }else if(flag == "d" || flag == "debug"){
	report("In debug mode",-2);
	reportingLevel = -2;
      }else if(flag == "rl" || flag == "reportinglevel"){
	i++;
	if(i==argc) break;
	try{
	  reportingLevel = stoi(argv[i]);
	}catch(const invalid_argument& e){
	  cout<<"No number for reporting level flag given"<<endl;
	  i--;
	}
      }else if(flag == "usingPreCompiledHeaders"){
	i++;
        if(i==argc) break;
	string tmp = string(argv[i]);
	usingPreCompiledHeaders = (tmp == "true" || tmp == "t" || tmp == "1");
      }else if(flag == "q" || flag == "quiet"){
	reportingLevel = 122;
      }else if(flag == "x" || flag == "execute"){
	executeProg = false;
	i++;
	if(i==argc) break;
	if(!(string(argv[i]) == "true" || string(argv[i]) == "false")){//check if they are setting it to true or false, or if they just passed in execute
	  i--;
	  continue;
	}
	executeProg = string(argv[i]) == "true";
      }else if(flag == "c" || flag == "cppFileName"){
	i++;
	if(i==argc) break;
	cppFileName = string(argv[i])+".cpp";
	if(executableFileName == "")
	  executableFileName = string(argv[i])+".o";
      }else if(flag == "nocomp"){
	compileProg = false;
      }else if(flag == "o" || flag == "outFileName"){
	compileProg = true;
	i++;
	if(i==argc) break;
	executableFileName = string(argv[i])+".o";
      }else if(flag == "lint" || flag == "l"){
	compileProg = false;//if youre linting, you dont actually need to run
	executeProg = false;
	lint = true;
	SAVE_LINE_COMMENTS = true;//need to track comments
	SAVE_BLOCK_COMMENTS = true;
	SAVE_WHITESPACE = true;
	setFlag("AUTO_FUNCTIONS", false);
      }else if(flag == "clang"){
	usingClang = true;
      }else if(flag == "gcc"){
	usingClang = false;
      }else if(flag == "b" || flag == "benchmark"){
	benchmarkTime = true;
      }else if(flag == "format"){
	format = true;
	parse = false;
	compileProg = false;
        executeProg = false;
	SAVE_LINE_COMMENTS = true;
        SAVE_BLOCK_COMMENTS = true;
        SAVE_WHITESPACE = true;
      }else if(flag == "version"){
	cout<<TWINE_VERSION<<endl;
	doneOtherThings = true;
      }else if(flag == "intversion"){
	exit(TWINE_VERSION_INT);//returns the version so its easy to progamatically get the version
      }else if(flag == "setall"){
	setAllLintFlags();
	//set parser flags
      }else if(flag == "set"){//TODO doent work right
	bool foundOne;
	do{
	  foundOne = false;
	  i++;
	  if(i==argc) break;
	  foundOne = setFlag(argv[i], true);
	  if(foundOne){
	    if(i+1 != argc)
	      try{
		int value = stoi(argv[i+1]);//throws if i+1 isnt a number
		foundOne = setLintFlag(argv[i],value);//wont happen if stoi throws error
		i++;
	      }catch(const invalid_argument& e){
		foundOne = setLintFlag(argv[i]);
	      }
	  }
	}while(foundOne);
      }else if(flag == "unset"){//unset parser flags or linter flag
	bool foundOne;
        do{
	  foundOne = false;
          i++;
          if(i==argc) break;
          /*for(int j = 0;j<sizeof(compileFlags)/sizeof(compileFlags[0]);j++){
            if(argv[i] == compileFlags[j].name){
              compileFlags[j].enabled = false;
              foundOne = true;
            }
	    }*/
	  foundOne = setFlag(argv[i], false);
	  //TODO linter flags
	  if(!foundOne)
	    foundOne = setLintFlag(argv[i], false);
	  if(!foundOne){
	    cout<<"Couldn't find flag: "<<argv[i]<<endl;
	  }
        }while(foundOne);
	i--;
      }else if(flag == "args"){
	for(i++; i < argc; i++){
	  argsForProg += string(argv[i]) + ' ';
	}
      }else if(flag == "file" || flag == "f"){
	i++;
	if(i==argc) break;
        inFiles.push_back(string(argv[i]));
      }else if(resolveFlag(flag)){
      //Do nothing
      }else{
	report("Invalid flag in main: "+flag, 2);
	report("use -help for usage", 2);
      }
    }else{      
      //add file to compile list
      if(arg.size() > 3 && arg.substr(arg.size()-3, arg.size()) == ".tw")
	inFiles.push_back(OGarg);
      else
	argsForProg += '\'' + OGarg+"' ";
    }
  }
  //get path to main file
  if(inFiles.size() != 0){
    int found = 0;
    found = inFiles.front().find_last_of("/\\");//first file in is 'main' file
    if(found != -1)
      pathToFile = inFiles.front().substr(0,found);//used to get includes to file
    mainFileName = inFiles.front().substr(found+1);
  }
}
void help(string what){
  string ogWhat = what;
  what = toLower(what);
  cout<<endl;
  doneOtherThings = true;
  if(what == ""){
    cout<<"To compile and execute a program, pass the file name to this program:   ./twine FILE_NAME [OPTIONS]"<<endl;
    cout<<"You can also lint a file by using the -lint flag:                       ./twine FILE_NAME -lint"<<endl;
    cout<<"To enter the interpreter, pass the -interp flag:                        ./twine -interp"<<endl;
    cout<<endl;
    cout<<"To find out more about a built in function, symbol, or flag, enter it after -help"<<endl;
    cout<<"Example:    ./twine -help randStr"<<endl;
    cout<<endl;
    cout<<"To get a list of built in functions, try:         ./twine -help functions"<<endl;
    cout<<"To get a list of built in symbols, try:           ./twine -help symbols"<<endl;
    cout<<"To get a list of built in compiler options, try:  ./twine -help compileFlags"<<endl;
    cout<<"To get a list of built in classes, try:           ./twine -help nameSpaces"<<endl;
    cout<<"To get help about how to use the linter, try:     ./twine -help lint"<<endl;
    cout<<"\nTo get an idea of the twine, look at the tutorials and examples in their respective directors"<<endl;
    cout<<endl;
    cout<<"To set a compiler flag, use the set keyword"<<endl;
    cout<<"Example:    ./twine first.tw -set SAVE_LINE_COMMENTS\n"<<endl;
    cout<<"To unset a compiler flag, use the unset keyword"<<endl;
    cout<<"Example:    ./twine first.tw -unset DEFUALT_RETURN"<<endl;
    cout<<'\n';
    cout<<"To get a full list of command-line options, use options"<<endl;
    cout<<"./twine -help options"<<endl;
    cout<<endl;
    return;
  }
  if(what == "functions"||what=="funcs"){//list all functions
    for(int i = 0;i<sizeof(builtInFunctions)/sizeof(builtInFunctions[1]);i++)
      cout<<builtInFunctions[i].toString()<<"\n";
    return;
  }else if(what == "interpfuncs" || what == "interpretedfunctions"){
    for(int i = 0;i<sizeof(interpFunctions)/sizeof(interpFunctions[0]);i++)
      cout<<interpFunctions[i].toString()<<"\n";
    return;
  }else if(what == "symbols"){//list data types and predefined vars
    cout<<"Pre-defined varriables:"<<endl;
    for(int i = 0;i<sizeof(builtInValues)/sizeof(builtInValues[1]);i++)
      cout<<builtInValues[i].toString()<<"\n";
    cout<<"\nData types: \n";
    for(unsigned i = 0;i<sizeof(symbols)/sizeof(symbols[1]);i++)
      cout<<symbols[i].toString()<<"\n";
    return;
    
  }else if(what == "compileflags" || what == "compilerflags" || what == "flags"){//list compiler flags
    cout<<"To set flags use 'set' follwed by the flags to enable"<<endl;
    cout<<"To unset flags use 'unset' follwed by the flags to disable\n"<<endl;
    for(unsigned i = 0;i<sizeof(compileFlags)/sizeof(compileFlags[1]);i++)
      cout<<compileFlags[i].toString()<<"\n";
    return;
  }else if(what == "nameSpaces" || what == "classes" || what == "objects"){
    for(unsigned i = 0;i<sizeof nameSpaces/sizeof(nameSpaces[1]);i++)
      cout<<nameSpaces[i].toString()<<"\n--------------------------------------------------------------\n\n";
    return;
  }else if(what == "interp" || what == "interpreter"){
    cout<<"Use the -interp flag to enter interpreter mode:    ./twine -interp"<<endl;
    cout<<"List of interpreter only functions: \n";
    for(unsigned i = 0;i<sizeof interpFunctions/sizeof interpFunctions[0];i++){
       cout<<interpFunctions[i].name<<": "<<interpFunctions[i].toString()<<"\n";
    }
    return;
  }else if(what == "options"){
    for(unsigned i = 0;i<sizeof commandLineOptions/sizeof commandLineOptions[0];i++){
       cout<<commandLineOptions[i].toString()<<"\n";
    }
    return;
  }else if(what == "reserved" || what == "keywords"){
    cout<<"Keywords are words reserved by the language for specific purposes. They can not be used as names of anything(vars, function, classes, etc)"<<endl;
    for(unsigned i = 0;i<(sizeof RESERVED/sizeof RESERVED[0])-1;i++){
       cout<<RESERVED[i]<<"\n";
    }
    cout<<RESERVED[(sizeof RESERVED/sizeof RESERVED[0])-1]<<"\n";
    return;
  }
  for(unsigned i = 0;i<sizeof(builtInFunctions)/sizeof(builtInFunctions[1]);i++){//search built in functions
    if(toLower(builtInFunctions[i].name) == what){
      cout<<builtInFunctions[i].toString()<<"\n";
      return;
    }
  }
  
  for(unsigned i = 0;i<sizeof(builtInValues)/sizeof(builtInValues[1]);i++){//search built in pre-def values
    if(toLower(builtInValues[i].name) == what){
      cout<<builtInValues[i].toString()<<"\n";
      return;
    }
  }
  
  for(unsigned i = 0;i<sizeof(compileFlags)/sizeof(compileFlags[2]);i++){//search compile flags
    if(toLower(compileFlags[i].name) == what){
      cout<<compileFlags[i].toString();
      return;
    }
  }
  for(unsigned i = 0;i<sizeof(nameSpaces)/sizeof(nameSpaces[3]);i++){//search data types(namespaces)
    if(toLower(nameSpaces[i].name) == what){
      cout<<nameSpaces[i].toString()<<"\n";
      return;
    }
  }
  if(what == "lint"){
    cout<<"Use -lint to enable the Twine linter";
    cout<<"This will look through a file you provide and search for adhearing to coding standards\n";
    cout<<"You can change what you want the linter to look for, and parameters for them using -set FLAG [value] and -unset FLAG [value]\n";
    cout<<"The flags are: \n";
    for(unsigned i = 0; i< sizeof linterFlags / sizeof linterFlags[0];i++){
      cout<<linterFlags[i].toString()<<'\n';
    }
    cout<<endl;
    return;
  }
  if(what == "format"){
    cout<<"Twine has a built in formatter\n";
    cout<<"Use -format to use\n";
    cout<<"./twine file.tw -format"<<endl;
    cout<<"The flags are: \n";
    for(unsigned i = 0; i< sizeof formaterFlags / sizeof formaterFlags[0];i++){
      cout<<formaterFlags[i].toString()<<'\n';
    }
    cout<<endl;
    return;
  }
  
  cout<<"Unknown what in help: "<<ogWhat<<'\n';
  cout<<"Try -help with nothing else to look at more options"<<endl;
}
void about(){
  doneOtherThings = true;
  cout<<"Twine"<<endl;
}


void freeProg(){
  high_resolution_clock::time_point start = high_resolution_clock::now();
  delete Prog;

  for(unsigned i = 0; i < TwineFunks.size(); i++){
    delete TwineFunks[i];
  }
  for(unsigned i = 0; i < TwineObjects.size(); i++){
    delete TwineObjects[i];
  }
  
  duration<double> time_span = duration_cast<microseconds>( high_resolution_clock::now() - start );
  report("Time to clean: "+to_string((long double) time_span.count())+" seconds.",0);
}

/*void handler(const int sig) {//TODO get rid of stack trace that occures before __userMain__
  void *array[12];
  size_t size;

  // get size of stack
  size = backtrace(array, 12);

  // print out all the frames to stderr
  fprintf(stderr, "Error:  %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);//STDERR_FILENO
  exit(sig);
  }*/
#include "includes/error.hpp"


#if defined(unix)
void segfaultHandler(int signal, siginfo_t *si, void *arg){
  cout<<"Segfualt cuased abort; adress "<<si->si_addr<<endl;
  printStack(0,1);
  exit(signal);
}
#else
void segfaultHandler(int signal, void * si, void *arg){
    cout<<"Segfualt cuased abort; signal "<<signal<<endl;
    printStack(0,1);
    exit(signal);
}

#endif

void __finish__(){}
