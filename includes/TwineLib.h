

#ifndef TWINELIB_BASIC
#define TWINELIB_BASIC

#include <fstream>//read, write, file IO
#include <iostream>//cout, cin
#include <string>
#include <math.h>//pow, sqrt
#include <iomanip> //#include <iomanip>


#include "Object.h"
#include "__ANY__.h"

using namespace std;

/*template<typename T> vector<T> :: vector<T> sub(const int start, const int end){
  if(start < 0 || start < end || end > this->length()){
    throw invalid_argument("Range error");
  }
  vector<T>::const_iterator start_itt = this->begin() + start;
  vector<T>::const_iterator end_itt = this->begin() + end;
  vector<T> newVec(start_itt, end_itt);
  return newVec;
  }*/

template<typename T>
vector<T> sub(const vector<T>& vec, const int start, const int end){
  if(start < 0 || start < end || end > vec.length()){
    throw invalid_argument("Range error");
  }
  vector<T> newVec( vec.begin() + start, vec.begin() + end);
  return newVec;
}
template<typename T>
vector<T> sub(int start, int end, const vector<T>& vec, const int step=1){
  if(start < 0)
    throw invalid_argument("Range error: start value must be greater than zero, "+to_string(start)+" given");
  if(start > end)
    throw invalid_argument("Start value ("+to_string(start)+" must be less then end value ("+to_string(end)+")");
  if(end >= vec.size())
    throw invalid_argument("Range error: end value must be less then vector size. Vector size: "+to_string(vec.size())+". Given value: "+to_string(end));//fix error msgs
  if(!step)
    throw invalid_argument("Step size can not be zero");
  //vector<T> newVec( vec.begin() + start, vec.begin() + end);
  vector<T> newVec;newVec.reserve(abs((end-start)/step));
  if(step > 0){//go forward
    for(;start < end; start+=step)//TODO add support for negitive step/start/end
      newVec.push_back(vec[start]);
  }else{
    end-=1;
    for(;end>=start; end+=step){
      newVec.push_back(vec[end]);
    }
  }
  return newVec;
}
template<typename T>
vector<T> sub(int start, const vector<T>& vec, const int step=1){
  return sub(start, vec.size()-1, vec, step);
}
string sub(int start, int end, const string str, const int step=1){
  if(start < 0)
    throw invalid_argument("Range error: start value must be greater than zero, "+to_string(start)+" given");
  if(start > end)
    throw invalid_argument("Start value ("+to_string(start)+" must be less then end value ("+to_string(end)+")");
  if(end >= str.size())
    throw invalid_argument("Range error: end value must be less then string size. String size: "+to_string(str.length())+". Given value: "+to_string(end));//fix error msgs
  if(!step)
    throw invalid_argument("Step size can not be zero");
  stringstream ss;
  if(step > 0){
    for(;start < end; start+=step){
      ss<<str[start];
    }
  }else{
    for(;end >= start; end+=step)
      ss<<str[end];
  }
  return ss.str();
}

string sub(int start, string str, const int step = 1){
  return sub(start, str.length()-1, str, step);
}

vector<int> sub(int start, int end, const int step = 1){
if(start < 0)
    throw invalid_argument("Range error: start value must be greater than zero, "+to_string(start)+" given");
  if(start > end)
    throw invalid_argument("Start value ("+to_string(start)+" must be less then end value ("+to_string(end)+")");
  //if(end >= base)
  //throw invalid_argument("Range error: end value must be less then base. Base: "+to_string(base)+". Given value: "+to_string(end));//fix error msgs
  if(!step)
    throw invalid_argument("Step size can not be zero");

  vector<int> ret = vector<int>();
  ret.reserve(abs((start-end)/step));
  if(step > 0){
    for(;start < end; start+=step)
      ret.emplace_back(start);
  }else{
    for(;end >= start; end+=step)
      ret.emplace_back(end);
  }
  return ret;
}

__ANY__ sub(int start, int end, __ANY__ var, int step = 1){
  return var.sub(start, end, step);
}

__ANY__ sub(int start, __ANY__ var, int step = 1){
  return sub(start, var.size(), var, step);
}

int system(const string command){return WEXITSTATUS(system(command.c_str()));}//TODO, add options to get string output, not wait, etc
//#define System(command) (WEXITSTATUS(system(command.c_str())))


void fualt(){*(int*)0 = 0;}//Used to cuased segfualt

__ANY__ sort(__ANY__ x, string mode=""){//should this be a member func of __ANY__?
  if(x.type == "_V")
    std::sort(((vector<__ANY__>*)x.ptr)->begin(),((vector<__ANY__>*)x.ptr)->end());
  if(x.type == "_S")
    return x; //TODO
  return x;
}
template<typename T> bool _LESS(T a, T b){ return a<b;}
//bool _LESS(int a, int b){ return a<b;}
template<typename T> vector<T> sort(vector<T> data){
  std::sort(data.begin(), data.end(), _LESS<T>);
  return data;
}
__ANY__ list(__ANY__ l){//should use ref
  __ANY__ b;
  b.type = "_V";
  b.ptr = new vector<__ANY__>;
  ((vector<__ANY__> *)b.ptr)->reserve(4);
  ((vector<__ANY__> *)b.ptr)->emplace_back(l);
  return b;
}
__ANY__ list(){
  __ANY__ b;
  b.type = "_V";
  b.ptr = new vector<__ANY__>;
  ((vector<__ANY__> *)b.ptr)->reserve(4);
  return b;
}
double pow(double exponent=1.0){//double pow(double, double) is defined in math.h
  return pow(2.71828182845904523536, exponent);
  //return pow(M_E, exponent);
}

/*__ANY__ List(initializer_list<__ANY__> l){//TODO
  __ANY__ b;
  b.type = "_V";
  vector<__ANY__> * tmp = new vector<__ANY__>;
  for(const __ANY__ *i = l.begin(); i != l.end(); i++){
    tmp->push_back(*i);
  }
  b.ptr = tmp;
  }*/

//TODO check for errors
int toInt(const string s,int base = 10){//TODO just call it int?
#ifndef TWINE_QUIET
  try{
#endif
  return stoi(s,nullptr,base);
#ifndef TWINE_QUIET
  }catch(const overflow_error& e){
    //cout<<"The string you are tying to conver to an int using toInt is too big to be an integer. String: "<<s<<"\n";
  }catch(const invalid_argument& e){
    //cout<<"The string you are tying to conver to an int using toInt is not a number. String: "<<s<<"\n";
  }
#endif
  return -1;
}
int toInt(const __ANY__& a,int base = 10){
  if(a.type == "_S"){
    return toInt((*(string *)a.ptr),base);
  }else{
    return (int)a.toNum();//TODO change of base
  }
}
int toInt(const int& i){return i;};
int toInt(const bool& b, int base = 10){return b;}
//TODO check for errors
double toDouble(string s,int base = 10){//TODO base change
  return stod(s,nullptr);
}
int toDouble(__ANY__ a,int base = 10){//TODO base change
  if(a.type == "_S"){
    return toDouble((*(string *)a.ptr),base);
  }else{
    return a.toNum();
  }
}
double toDouble(const bool& b, int base = 10){return b;}


string toString(const int& num){return to_string(num);}

string toString(const bool& b){
  if(b)
    return "true";
  return "false";
}

string toString(const double pol, int decmalCount = -1){//TODO 
  //TODO
  ostringstream oss;
  string tmp;
  if(decmalCount < 0){
    oss << pol;
    tmp = oss.str();
    if(tmp.find_first_of('.') == string::npos){
      tmp+=".0";
    }
  }else{
    oss << setprecision(decmalCount)<<pol;
    tmp = oss.str();
  }
  return tmp;
}
template<typename T, size_t N> string toString(const array<T, N>& arr, long start = 0, const long end = -1){//repeated code in print
  string ret = "{";
  for(;start < N && (end == -1 || start <= end);){
    ret += toString(arr[start]);
    start++;
    if(start < N && (end == -1 || start <= end))
      ret += ',';
  }
  ret += '}';
  return ret;
}//missing vector

string toString(const __ANY__& a){return a.toString();}
/*template<class T> string toString(const T& data){
  return data.toString();
  }*/
//end casts

double max(const __ANY__ &x){
  if(x.type != "_V") return x.toNum();
  double max = ((vector<__ANY__>*) x.ptr)->front().toNum();
  for(vector<__ANY__>::iterator i = ((vector<__ANY__>*) x.ptr)->begin()+1;i != ((vector<__ANY__>*) x.ptr)->end();i++){
    if(i->toNum() > max) max = i->toNum();
  }
  return max;
}
double min(const __ANY__ &x){
  if(x.type != "_V") return x.toNum();
  double min = ((vector<__ANY__>*) x.ptr)->front().toNum();
  for(vector<__ANY__>::iterator i = ((vector<__ANY__>*) x.ptr)->begin()+1;i != ((vector<__ANY__>*) x.ptr)->end();i++){
    if(i->toNum() < min) min = i->toNum();
  }
  return min;
}
int len(const __ANY__ &x){//TODO need this?
  if(x.type != "_V")
    return 1;
  return x.toNum();
}
int len(string x){
  return x.length();
}
double abs(const __ANY__ &x){
  if(x.toNum()<0) return -x.toNum();
  return x.toNum();
}
double round(double x){
  x+=.5;
  return ((int) x);
}
bool canOutputColor(){//TODO turn into constant at beggining of compile
  string terminal = getenv("TERM");
  if(terminal.substr(0,5) == "xterm") return true; //TODO
  if(terminal.substr(0,2) == "VT") return true; //TODO
  if(terminal.substr(0, 14) == "gnome-terminal") return true; //TODO
  if(terminal == "linux") return true;
  return false;
}

//TODO, get forground/background color can be upgraded with more colors using RGB values
//and string_to int values (user passes in RGB values as parameters)
int getForgroundColorCode(const string& color){
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

int getBackgroundColorCode(const string& color){
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

template<typename T> void print(const vector<T>& arr, const string& text_color="", const string& background_color = "", long start = 0, const long end = -1){//TODO
  cout<<"{";
  for(;start < arr.size() && (end == -1 || start <= end);){
    cout<<arr[start];
    start++;
    if(start < arr.size() && (end == -1 || start <= end))
      cout<<", ";
  }
  cout<<"}"<<endl;
}

//void print(const value_type&){}

template<typename T, size_t N> void print(const array<T, N>& arr, const string& text_color="", const string& background_color = "", long start = 0, const long end = -1){//TODO
  cout<<"{";
  for(;start < N && (end == -1 || start <= end);){
    cout<<arr[start];
    start++;
    if(start < N && (end == -1 || start <= end))
      cout<<", ";
  }
  cout<<"}"<<endl;
}

void print(const __ANY__& data, const string& text_color = "", const string& background_color = ""){
  static bool canColor = canOutputColor();
  const string __tmp__ = data.toString();
  if(!canColor){
    cout<<__tmp__<<'\n';
  }else{
    cout<<"\033["<<getForgroundColorCode(text_color)<<";"<<getBackgroundColorCode(background_color)<<"m"<<__tmp__<<"\033[0m\n";
  }
}

template<typename T> void print(const T& data, const string& text_color = "", const string& background_color = ""){
  static bool canColor = canOutputColor();//TODO can make this one var for both functions?
  if(!canColor){
    cout<<data<<'\n';
  }else{
    cout<<"\033["<<getForgroundColorCode(text_color)<<";"<<getBackgroundColorCode(background_color)<<"m"<<data<<"\033[0m\n";
  }
}

void print(){
  cout<<endl;
}

//void print(const string& str, const string& end, int background_color, int text_color){//TODO what standard should i use for print?
void print(const string& str, const string& text_color, const string& background_color = ""){
  //should it be one print like python, 2 like java, standard for color, etc?
  static bool canColor = canOutputColor();//TODO can make this one var for both functions?
  if(!canColor){
    cout<<str<<'\n';
  }else{
    cout<<"\033["<<getForgroundColorCode(text_color)<<";"<<getBackgroundColorCode(background_color)<<"m"<<str<<"\033[0m\n";
  }
}
void printn(const string& str, const string& text_color, const string& background_color = ""){
  //should it be one print like python, 2 like java, standard for color, etc?
  if(!canOutputColor()){
    cout<<str<<'\n';
  }else{
    cout<<"\033["<<getForgroundColorCode(text_color)<<";"<<getBackgroundColorCode(background_color)<<"m"<<str<<"\033[0m";
  }
}
void print(const string& str){
  cout<<str<<'\n';
}
void print(const char a[]){
  cout<<a<<'\n';
}
void printn(const char a[]){
  cout<<a;
}
void printn(const __ANY__& a){
  cout<<a.toString();
}
void print(const int str){
  print(to_string((long long)str));
}
void print(const int64_t i){
  print(to_string(i));
}
void print(const double str){
  print(to_string((long double)str));
}
void print(long unsigned i){//TODO--colors
  cout<<i<<'\n';
}
void printn(const string str){
  cout<<str;
}
void printn(const int str){
  printn(to_string((long long)str));
}
void printn(const double str){
  printn(to_string((long double)str));
}
void print(const string * str){
  cout<<*str<<'\n';
}
void printn(const string * str){
  cout<<*str;
}
void print(const void * ptr){cout<<ptr<<'\n';}
void printn(const void * ptr){cout<<ptr;}
//template<typename T> void print(const T& str){cout<<str<<'\n';}
string input(const string msg = ""){
  cout<<msg<<flush;
  string str;
  getline(cin, str); 
  return str;
}
string input(const string* msg){
  cout<<*msg;
  string str;
  getline(cin, str); 
  return str;
}
int randInt(int min = 0, int max = RAND_MAX){
  return (min+rand()%(max-min+1));
}
string randStr(int length = 10){//randString?
  char src[length];
  src[length] = '\0';
  while(--length > -1)
    src[length] = (rand() % 94) + 32;
  string out(src);
  return out;
}
double randDouble(double min = 0.0, double max = 1.0){//TODO
  long long tmp = (long long)(RAND_MAX)*rand()+rand();//trying to get 64 bit double percition
  long long tmp2 = (long long)(4294967296)*RAND_MAX + (long long)RAND_MAX;
  return (min+(tmp/(tmp2/(max-min))));
}
void sleep(double sleepTime){  this_thread::sleep_for(chrono::microseconds(int(sleepTime*1000000)));  }//TODO another is already included

bool write(const string fileName, const string text, const bool append = false){//TODO readFile/writeFile?
  ofstream outputFile;
  if(append){outputFile.open(fileName.c_str(),ios_base::app);
  }else{outputFile.open(fileName.c_str());}

  if(!outputFile.is_open())
    return false;
  outputFile<<text;
  outputFile.close();
  return true;
}
string read(const string fileName){
  ifstream inputFile(fileName.c_str());
  if(!inputFile.is_open()) return "";

  string text;
  string line;
  while(getline(inputFile,line)){text+=line+'\n';}
  //text.erase(text.size()-1);//erase last \n
  text.pop_back();
  inputFile.close();
  return text;
}
;

template<typename T> 
void insert(vector<T>& in, const long pos, const T& thing){
  in.insert(in.begin()+pos, thing);
}

#endif//TWINELIB_BASIC
