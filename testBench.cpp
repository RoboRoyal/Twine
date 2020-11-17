
#include <chrono>//high_resolution_clock

#include "includes/Channel.h"
//#include "includes/2Channel.h"
#include "includes/runnable.h"
int testSize = 100*100*100;


using namespace std::chrono;

void testThread();


void myTestFunk(){
  cout<<"test global"<<endl;
}

class mine{
public:
  void myTestFunk(){
  cout<<"class funk"<<endl;
}
  void callMe(){
    ::myTestFunk();
    this->myTestFunk();
  }
};

void testThing(){
  vector<int> A = {2,3,5};
  cout<<"equal? "<<(A==vector<int>({2,3,5}))<<endl;
  //cout<<123<<endl;
  //mine me = mine();
  //me.callMe();
  //testThread();
}

//ChannelTwo<high_resolution_clock::time_point> chan;
/*Channel<high_resolution_clock::time_point> chan;

runnable<high_resolution_clock::time_point> one;
runnable<high_resolution_clock::time_point> two;

void amProducer(){
  for(int i = 0; i < testSize; i++){
    high_resolution_clock::time_point start = high_resolution_clock::now();
    one.myChannel->write(start);
  }
  cout<<"Done producer"<<endl;
}

void amConsumer(){
  double max = 0;
  double length = 0;
  for(int i = 0; i<testSize; i++){
    //high_resolution_clock::time_point out = me.read();
    high_resolution_clock::time_point out = one.myChannel->read();
    duration<double> time_span = duration_cast<microseconds>( high_resolution_clock::now() - out);
    //cout<<"Durration: "<<to_string((long double) time_span.count())<<endl;
    if(time_span.count() > max)
      max = time_span.count();
    length+=time_span.count();
  }
  cout<<"Done consumer"<<endl;
  cout<<"Total: "<<to_string((long double) length)<<endl;
  cout<<"Average: "<<to_string((long double) length/testSize)<<endl;
  cout<<"Max: "<<to_string((long double) max)<<endl;
}


void foo(producer<high_resolution_clock::time_point> &me){
  cout<<"This is foo thread"<<endl;
  for(int i = 0; i < testSize; i++){
    high_resolution_clock::time_point start = high_resolution_clock::now();
    //me.write(start);
    chan.write(start);
    //sleep(.01);
  }
  cout<<"Done producer"<<endl;
}
//void bar(Channel<string> me){
void bar(consumer<high_resolution_clock::time_point> &me){
  cout<<"This is bar thread"<<endl;
  double max = 0;
  double length = 0;
  for(int i = 0; i<testSize; i++){
    //high_resolution_clock::time_point out = me.read();
    high_resolution_clock::time_point out = chan.read();
    duration<double> time_span = duration_cast<microseconds>( high_resolution_clock::now() - out);
    //cout<<"Durration: "<<to_string((long double) time_span.count())<<endl;
    if(time_span.count() > max)
      max = time_span.count();
    length+=time_span.count();
  }
  cout<<"Done consumer"<<endl;
  cout<<"Total: "<<to_string((long double) length)<<endl;
  cout<<"Average: "<<to_string((long double) length/testSize)<<endl;
  cout<<"Max: "<<to_string((long double) max)<<endl;
}

void testThread(){
  high_resolution_clock::time_point start = high_resolution_clock::now();
  one = runnable<high_resolution_clock::time_point>(amProducer, true, false);
  two = runnable<high_resolution_clock::time_point>(amConsumer, false, false);
  cout<<"Done starting threads"<<endl;
  one.safeEnd();
  two.safeEnd();
  /*Channel<high_resolution_clock::time_point> me2;// = ChannelTwo<high_resolution_clock::time_point>();
  producer<high_resolution_clock::time_point> as = producer(me2);
  consumer<high_resolution_clock::time_point> ad = consumer(me2);
  chan.addProducer();
  thread one(foo, ref(as));
  thread two(bar, ref(ad));
  //ChannelTwo<string> me;
  //thread three(trial, me);
  //thread four(thing, me);*/
  /*cout<<"Done initiing"<<endl;
  one.join();
  two.join();
  //three.join();
  //four.join();
  cout<<"done both"<<endl;
  duration<double> time_span = duration_cast<microseconds>( high_resolution_clock::now() - start);
  cout<<"Total time: "<<time_span.count()<<endl;
}*/

void testExp(){
  //get lex from file
  vector<TokenData>* tokens = getTokenisedFile("third.tw");
  for(vector<TokenData>::iterator i = tokens->begin(); i != tokens->end(); i++){
    report(TokenData::getTokenTypeName(i->tokenType)+": "+i->tokenText+": at "+to_string((long long)i->charPos)+": "+to_string((long long)i->line),-1);
  }
  //set sym
  interpMode = true;
  tokenList = tokens;
  currentVars = new scope();
  localVars = new vector<scope *>;
  globalVars = new scope();
  currentLineOn = false;
  Prog = new prog();
  currentClass = NULL;
  setIterators();
  initDefualtVars();
  setUpUserMain(Prog);
  currentFunk = Prog->functions.at(0);
  
  //send to parseExp
  expression3 * exp3 = parseExpression();
  cout<<"Got exp3"<<endl;
  //send to resolve2
  reportingLevel = -2;
  expression2 * exp2 = resolveExpression2(exp3, "num");
  expression2 * exp2_a = resolveExpression2(exp3, "string");
  expression2 * exp2_b = resolveExpression2(exp3, "");
  cout<<"Got exp2"<<endl;
  reportingLevel = 2;
  //trans and print
  out = "";
  TransExp(exp3);
  cout<<"Got Trans out 3 "<<endl;
  cout<<out<<endl;
  print(interpExpression(exp3));
  reportingLevel = 2;
  
  out = "";
  TransExp(exp2);
  cout<<"NUM: ";
  cout<<out<<endl;
  print(interpExpression(exp2));

  out = "";
  TransExp(exp2_a);
  cout<<"String: ";
  cout<<out<<endl;
  print(interpExpression(exp2_a));

  out = "";
  TransExp(exp2_b);
  cout<<"__ANY__: ";
  cout<<out<<endl;
  print(interpExpression(exp2_b));
  
}



/*
while(accept("[")){
        //new big atom [
        bigAtom * openAtom = new bigAtom();openAtom->type = bigAtom::OP; openAtom->op = lastSym->tokenText; EXP->bigAtoms->push_back(openAtom);
        //new big atom exp
        bigAtom * expAtom = new bigAtom();expAtom->type = bigAtom::ATOM;
        atom * expHolder = new atom();expHolder->type="exp3";
        expHolder->exp3 = parseExpression("num");
        expAtom->a = expHolder;
        EXP->bigAtoms->push_back(expAtom);
        //new big atom ]
        expect("]");
        bigAtom * closeAtom = new bigAtom();closeAtom->type = bigAtom::OP; closeAtom->op = lastSym->tokenText; EXP->bigAtoms->push_back(closeAtom);
        //beingCalledOn shouldnt change from this
        isStaticCall = false;
        //needOP = false;
 }


while(accept("[")){//this replaces the use of [] with calls to at(); 
  expression2 * atExp = parseExpression("num");//get value from inside []
  expect("]");//get closing part of []
  isStaticCall = false;
  
  funkCall * atCall = new funkCall();atCall->name = "at";
  atom * expHolder = new atom();expHolder->type="funkCall";
  atCall->parameters2->push_back(atExp);
  expHolder->fCall = atCall;

  bigAtom * openAtom = new bigAtom();openAtom->type = bigAtom::OP; openAtom->op = "."; EXP->bigAtoms->push_back(openAtom);
  bigAtom * expAtom = new bigAtom();expAtom->type = bigAtom::ATOM; expAtom->a = expHolder; EXP->bigAtoms->push_back(expAtom);
  
 }

*/

/*void download2(string url){cout<<"Downloading file: "<<url<<endl;}

bool isValidFileType(const string& file);


void parseImport2(string name){
  string pathToFile;
  bool isCpp;
  if(fileExists(name)){
    pathToFile = name;
  }else if(name.substr(0,8) == "https://"){
    download2(name);
    pathToFile = "downloadedImport/"+name.substr(name.find_last_of("/")+1);
  }else{
    error("Unkown import type");
  }
  string ext = pathToFile.substr(pathToFile.find_last_of('.'));
  if(ext == ".cpp" || ext == ".h"){
    isCpp = true;
  }else if(ext == ".tw"){
    isCpp = false;
  }else{
    //error("Unknown file type");
    cout<<"Unknown file type"<<endl;
  }
  cout<<name<<endl;
  cout<<"Is cpp: "<<isCpp<<endl;
  cout<<"Path: "<<pathToFile<<endl;
  cout<<"--------------------------"<<endl<<endl;
  if(!isCpp){//must be twine
    //add to files to lint
  }else{
    //will need to parse out and add to function defs
  }
}

*/

#include <unistd.h>

unsigned long long getTotalSystemMemory()
{
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    cout<<pages * page_size<<endl;
    return pages * page_size;
}

/*

void printList(vector<int> list){
  for(int i = 0; i<list.size();i++)
    cout<<list.at(i)<<", ";
  cout<<endl;
}

void testBench(string name){
  vector<int> list;

  for(int i = 0; i<10;i++)
    list.push_back(i);
  printList(list);
  list.pop_back();

  printList(list);
  list.pop_back();

  printList(list);
  cout<<"last element: "<<list.back();

}
*/



/*
void testBench(string name){
  for(int i = 0;i<sizeof(builtInFunctions)/sizeof(*builtInFunctions);i++){
    if(builtInFunctions[i].name == name){
      Funk * tmp = new Funk();
      tmp->name = name;
      tmp->returnType = builtInFunctions[i].returnType;
      vector<string> params = split(builtInFunctions[i].params,",");

      for(int x = 0;x<params.size();x++){
	vector<string> singleParam = split(params.at(x), " ");
        var * newVar = new var();
	
	cout<<"Type: "<<singleParam.at(0)<<" ";
	newVar->type = singleParam.at(0);
		
        if(split(singleParam.at(1),"=").size() == 2){
	  cout<<"Name: "<<split(singleParam.at(1), "=").at(0)<<" ";
	  newVar->name = split(singleParam.at(1), "=").at(0);
	  
          newVar->startingValue = stringToExp(split(singleParam.at(1),"=").at(1));
	  cout<<"Stating Value: "<<split(singleParam.at(1),"=").at(1)<<" ";
        }else{
	  cout<<"Name: "<<singleParam.at(1)<<" ";
	  newVar->name = singleParam.at(1);
	}
        tmp->parameters.push_back(*newVar);
	cout<<endl;
      }
      //TwineFunks.push_back(tmp);
      //return tmp;
      return;
    }
  }
}
*/
/*
for(int i = 0;i<sizeof(builtInFunctions)/sizeof(*builtInFunctions);i++){
    if(builtInFunctions[i].name == name){
      Funk * tmp = new Funk();
      tmp->name = name;
      tmp->returnType = builtInFunctions[i].returnType;
      vector<string> p = split(builtInFunctions[i].params,",");
      for(int x = 0;x<p.size();x++){
        var * newVar = new var();
        if(split(p.at(x),"=").size() == 2){
          newVar->type = split(p.at(x),"=").at(0);
          newVar->startingValue = stringToExp(split(p.at(x),"=").at(1));
          x++;
        }else{
          newVar->type = p.at(x);
        }
        tmp->parameters.push_back(*newVar);
      }
      TwineFunks.push_back(tmp);
      return tmp;
    }
  }
*/


/*
int getForgroundColorCode(string color){
  if(color == "black")
    return 30;
  if(color == "red")
    return 31;
  if(color == "green")
    return 32;
  if(color == "yellow")
    return 33;
  if(color == "blue")
    return 34;
  if(color == "magenta")
    return 35;
  if(color == "cyan")
    return 36;
  if(color == "white")
    return 37;
  return 39;
}

int getBackgroundColorCode(string color){
  if(color == "black")
    return 40;
  if(color == "red")
    return 41;
  if(color == "green")
    return 42;
  if(color == "yellow")
    return 43;
  if(color == "blue")
    return 44;
  if(color == "magenta")
    return 45;
  if(color == "cyan")
    return 46;
  if(color == "white")
    return 47;
  return 49;
}

const string WORKING_COLOR_TERM = "xterm";
void testBench(string text, string color = "", string back = ""){
  if(color == "" || getenv("TERM") != WORKING_COLOR_TERM){
    cout<<text<<"\n";
  }else{
    cout<<"\033["<<getForgroundColorCode(color)<<";"<<getBackgroundColorCode(back)<<"m"<<text<<"\033[0m\n";
  }
  //cout << "\033[1;31mbold red text\033[0m\n"<<endl;//This is for color text
  //cout<<getenv("TERM")<<endl;
}
*/



/*
expression2 * resolveAtoms(atom * left, atom * right, string OP){
  debug("resolveAtoms()");
  expression2 * EXP = new expression2;
  EXP->OP = OP;
  resolveAtom(left);
  resolveAtom(right);
  cout<<"Left type: "<<left->type<<" left helper: "<<left->helper<<" left converterd: "<<convertedType(left->helper)<<endl;
  cout<<"Right type: "<<right->type<<" right helper: "<<right->helper<<" right converterd: "<<convertedType(right->helper)<<endl;
  if(convertedType(left->helper) == convertedType(right->helper)){//if they are the same, were good
    EXP->helper = convertedType(right->helper);
  }else if(left->helper == "__ANY__" || right->helper== "__ANY__"){//if one is an any, they both have to be
    if(left->helper != "__ANY__")
      left->cast == "__ANY__";
    if(right->helper != "__ANY__")
      right->cast == "__ANY__";
    EXP->helper = "__ANY__";
  }else if(left->helper == "string" || right->helper== "string"){//if one is a string, they both have to be
    if(convertedType(left->helper) == "num"){
      left->cast == "num to string";
    }else{
      left->cast == "string";
    }
    if(convertedType(right->helper) == "num"){
      right->cast == "num to string";
    }else{
      right->cast == "string";
    }
    EXP->helper = "string";
  }else{
    throw "ERROR";
  }
  EXP->leftAtom = left;
  EXP->rightAtom = right;
  EXP->rightIsExp = false;
  EXP->leftIsExp = false;
  debug("resolveAtoms() done");
  return EXP;
}
*/

