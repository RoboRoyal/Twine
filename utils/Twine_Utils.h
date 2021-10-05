/*
 * Twine_Utils.h
 *
 *  Created on: Jul 1, 2018
 *      Author: Dakota
 */

#ifndef TWINE_UTILS_H_
#define TWINE_UTILS_H_

#include <string.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include <sys/stat.h>//fileExists


#define TWINE_WIERD_CONVERTIONS
#define TWINE_WIERD_ASSIGNMENTS

using namespace std;

string out = "";

void error(const string& msg, bool passable = false);
void warn(const string& msg);


bool connectFiles(vector<string>* inFiles, string* data);//reads and connects multiple files together
bool readFile(const char* fileName,string* data);//reads files in
void report(const string& text, int level);//print errors/warnings/info
bool writeFile(const char* fileName, string* text);//write out to files
bool appendFile(const char* fileName, string* text);
string toUpper(string str);
string toLower(string str);
bool strEql(const string& one, const string& two);
vector<string> split(string str, string sep);

#include "../Flags.h"


bool connectFiles(vector<string>* inFiles, string* data){
  for(vector<string>::iterator i = inFiles->begin(); i != inFiles->end(); i++){
    *data += string(string("\n#FILE ")+i->c_str()+'\n');//TODO this doenst properly insert a newline
    if(!readFile(i->c_str(),data)){
      report("Error reading file: "+*i,4);
      if(!FORCE)
	return false;
    }
  }
  return true;  
}

bool readFile(const char* fileName,string* data){
  //TODO increase speed
  //look at: https://stackoverflow.com/questions/17925051/fast-textfile-reading-in-c
  ifstream inFile;
  string line = "";
  inFile.open(fileName);
  if (!inFile) {
    report("Unable to open file: "+string(fileName),4);
    //if(FORCE){*data+='\n'; return true;}//poor
    return false;
  }else{
    report("Opening file: "+string(fileName), -2);
    while (getline(inFile, line)) {
      *data = *data + line + '\n';
    }
    if(*data == "")
      report("File was empty: "+string(fileName), -1);
    *data = *data + '\n';
    inFile.close();
  }
  report("Succsess reading file: "+string(fileName),-1);
  return true;
}

bool writeFile(const char* fileName, string* text){
  ofstream outFile;
  outFile.open(fileName);
  if(!outFile){
    report("Unable to open file: "+string(fileName),4);
    return false;
  }else{
    report("Writing file: "+string(fileName),-1);
    outFile<<*text;
    outFile.close();
  }
  return true;
}
bool appendFile(const char* fileName, string* text){
  ofstream outFile;
  outFile.open(fileName, ios_base::app);
 if(!outFile){
   report("Unable to open file: "+string(fileName),4);
   return false;
 }else{
   report("Opening file: "+string(fileName),-1);
   outFile<<*text;
   outFile.close();
 }
 return true;
}

void report(const string& text, int level){//0 = info, 2 = warning, 4 = error, -2 = debug
  if(level >= reportingLevel)
    cout<<text<<'\n';
}

string toUpper(string input){
    transform(input.begin(), input.end(), input.begin(), ::toupper);
    return input;
}

string toLower(string input){
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    return input;
}

bool strEql(const string& one, const string& two){
  if(toLower(one) == toLower(two))
    return true;
  return false;
}

vector<string> split(string str, string sep){
  char* cstr=const_cast<char*>(str.c_str());
  char* current;
  vector<std::string> arr;
  current=strtok(cstr,sep.c_str());
  while(current != NULL){
    arr.push_back(current);
    current=strtok(NULL, sep.c_str());
  }
  return arr;
}


#endif /* TWINE_UTILS_H_ */
