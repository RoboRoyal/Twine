
#include "Trans.h"

bool Trans(){//, vector<Funk> * ProgPtr){
  report("Trans compiling...",0);
  out = "";//all methods write output to out, which gets writen to output file
  tempStart();//makes new file/clear existing file, then writes out template
  TransForwardDeclair();
  if(!TransProg()){
    report("Trans compiling failed",2);
    return false;
  }
  tempEnd();
  //write();
  report("Trans compiling done",0);
  return true;
}
bool TransRunner(){//TODO extends
  out+="\n\n\n//NEW TEST THING\n";//va_end?
  //out+= "template<typename... Types>\n";
  //out+="template <typename... Ts>\n";
  out+="__ANY__ __ANY__::runObjFunk(const string& funcName, ... ){\n";
  out+="va_list args;\nva_start(args, funcName);\n";
  out+="try{";
  out+="auto tmp = runObjFunkHelper(funcName, args);";
  out+="va_end(args);\n return tmp;";
  out+="}catch(bad_alloc& e){";
  out+="va_end(args);\n";
  out+="throw runtime_error(\"Invalid call/invalid parameters to function '\"+funcName+\"' of class '\"+this->getType()+\"'. Dynamic calls require full parameters.\");}";//should it be cuaght?
  out+="}\n";
  out+="__ANY__ __ANY__::runObjFunkHelper(const string& funcName, va_list& args ){\n";
  //out+="va_list args;\nva_start(args, funcName);\n";
  for(vector<object *>::iterator i = Prog->classes.begin();i != Prog->classes.end();i++){
    const string objName = (*i)->name;
    out += "\tif(typeid("+objName+").name() == this->type){\n";
    for(vector<Funk *>::iterator j = (*i)->memberFunks.begin();j != (*i)->memberFunks.end();j++){
      out += "\t\tif(\"" + (*j)->name + "\" == funcName){\n";
      if((*j)->returnType.type != "void")
	out += "\t\t\treturn ";
      out += " (("+objName+" *)this->ptr)->"+(*j)->name+"(";
      for(vector<var>::iterator d = (*j)->parameters.begin();d != (*j)->parameters.end();){
	if(!(d->type == "int" || d->type == "double" || d->type == "bool")){
	  out += "* ("+d->type+" *) va_arg(args, void *)";
	}else if(d->type == "bool"){
	  out += "(bool) va_arg(args,int)";
	}else{
	  out += "va_arg(args,"+d->type+")";
	}
        d++;
        if(d != (*j)->parameters.end()){
          out += ", ";
        }
      }
      out += ");\n\t\t}\n";
      if((*j)->returnType.type == "void")
	out += "return __ANY__(0);\n";
    }
    out += "//TODO throw\n";//convert __ANY__ to parent object, call runObjFunk on new type?
    if((*i)->extends != ""){
      out+= "string tmp_s = this->type;\n    this->type = typeid("+(*i)->extends+").name();\n    auto tmp = runObjFunkHelper(funcName, args);\n";//how to pass on veradic function
      out+="this->type = tmp_s;\n    va_end(args); return tmp;";
    }
    //throw
    out += "\t}";
  }
  out += "throw invalid_argument(\"No function '\"+funcName+\"' found for object type '\"+this->getType()+\"'\");//TODO\n return __ANY__(0);\n";
  out += "}";
  return true;
}


void TransThing(){
  out+="\n\n\n//NEW TEST THING\n";
  out+="__ANY__ __ANY__::getObjVar(const __ANY__& obj, const string varName){\n";
  for(vector<object *>::iterator i = Prog->classes.begin();i != Prog->classes.end();i++){
    const string objName = (*i)->name;
    //out += objName+" T;\n";
    out += "if(typeid("+objName+").name() == obj.type){\n";
    for(vector<var *>::iterator j = (*i)->memberVars.begin();j != (*i)->memberVars.end();j++){
      out += "if(\"" + (*j)->name + "\" == varName){\n";
      out+= "return (("+objName+" *)obj.ptr)->"+(*j)->name+";\n}\n";
    }
    out += "throw invalid_argument(\"No class varriable \"+varName+\" in class "+objName+"\");}\n";
  }
  out+="throw invalid_argument(\"No class '\"+obj.type+\"' found. Looking for var '\"+ varName+\"'\");\n";
  out+="}";
}

bool tempStart(){//sets configuration and gets template file
  string tmp = "/*TWINE_VERSION = "+TWINE_VERSION+"*/";
  for(int i = 0; i < sizeof(compileFlags)/sizeof(compileFlags[0]);i++){
    if(compileFlags[i].enabled){
      tmp+= "#define "+compileFlags[i].name+'\n';
    }
  }
  //if(FORCE) tmp += "#define TWINE_FORCE\n";//TODO these guys switches
  //TODO includes
  tmp+="#define TWINE_SAFE\n";
  tmp+="#include \""+INSTALL_PATH+"includes/CONFIG."+((usingPreCompiledHeaders) ? "h.gch" : "h")+"\"\n";//I know im using .h.gch...
  tmp+="#include \""+INSTALL_PATH+"includes/EXTERN."+((usingPreCompiledHeaders) ? "h.gch" : "h")+"\"\n";
  //tmp+="#define int int32_t\n";//i think it should be int64_t as that is what ANY use4s as defualt
  tmp+="#include \""+INSTALL_PATH+"includes/__ANY__."+((usingPreCompiledHeaders) ? "h.gch" : "h")+"\"\n";
  tmp+="#include \""+INSTALL_PATH+"includes/TwineLib."+((usingPreCompiledHeaders) ? "h.gch" : "h")+"\"\n";
  tmp+="#include \""+INSTALL_PATH+"includes/Channel."+((usingPreCompiledHeaders) ? "h.gch" : "h")+"\"\n";
  tmp+="#include \""+INSTALL_PATH+"includes/runnable."+((usingPreCompiledHeaders) ? "h.gch" : "h")+"\"\n";
  tmp+="#include \""+INSTALL_PATH+"includes/TypeInfoLib."+((usingPreCompiledHeaders) ? "h.gch" : "h")+"\"\n";
  tmp+="#include \""+INSTALL_PATH+"includes/error."+((usingPreCompiledHeaders) ? "hpp.gch" : "hpp")+"\"\n";
  readFile((INSTALL_PATH + "temp1.cpp").c_str(),&tmp);
  //return writeFile(outFileName.c_str(),&tmp);//dont use write()
  return writeFile(cppFileName.c_str(),&tmp);
}


void TransForwardDeclair(){
  debugT("TransForwardDeclair()");
  if(Prog->globalIncludes.size()){
    out += "//Start of global includes\n";
    for(vector<include *>::iterator i = Prog->globalIncludes.begin();i != Prog->globalIncludes.end();i++){
      out += "#include \""+(*i)->fileName+"\"";
      
      out+="\n";
    }
    
    out += "//End of global includes\n";
  }
  
  //define classes
  if(Prog->classes.size()){
    out += "//Start of forward defs of classes\n";
    for(vector<object *>::iterator i = Prog->classes.begin();i != Prog->classes.end();i++){
      //out+="class "+(*i)->name+": public "+(*i)->extends+";\n";
      out+="class "+(*i)->name+";\n";
    }
    out += "//End of forward defs of classes\n";
  }
  //define branches
  if(Prog->branches.size()){
    out+="//Start of branches declair\n";
    for(vector<branch *>::iterator i = Prog->branches.begin();i != Prog->branches.end();i++){
      out+="class "+(*i)->name+";\n";//+"::public runnable<"+(*i)->type+">, public "+(*i)->extends+";\n";
    }
    out+="//End of branches";
  }
  //define function
  out += "//Forward decloration of user function\n";
  for(vector<Funk *>::iterator i = Prog->functions.begin()+1;i != Prog->functions.end();i++){//TODO use function
    /*out+=((*i)->STATIC?"static ":"")+(*i)->returnType + " "+(*i)->name+" (";
    for(vector<var>::iterator k = (*i)->parameters.begin();k != (*i)->parameters.end();){
      TransVar(&(*k));
      k++;
      if(k != (*i)->parameters.end()){
	out+=", ";
      }else{
	if((*i)->dynamicParamType != "")//add argLen and ... to end of ver funcs
	  out += ", int argLen = 0 ...";
      }
    }
    out+=");";
    out+='\n';
  }*/
    TransFunkKW(*i, true, true);
    out+=";\n";
  }
  debugT("TransForwardDeclair() done");
}
bool isBranch(const string& name){
  if(Prog->branches.size())
    for(int i = 0; i < Prog->branches.size(); i++)
      if(name == Prog->branches[i]->name) return true;
  return false;
}
void tempEnd(){
  out+="void __initiateStaticVars__(){";
  if(Prog->classes.size()){
    for(vector<object *>::iterator i = Prog->classes.begin();i != Prog->classes.end();i++){
      //TODO initiate static vars of classes
      for(vector<var *>::iterator V= (*i)->memberVars.begin(); V != (*i)->memberVars.end(); V++){
	if((*V)->STATIC && (*V)->startingValue != NULL){
	  out += (*i)->name + "::"+(*V)->name+" = ";
	  TransExp((*V)->startingValue);
	  out += ";\n";
	}
      }
    }
  }
  out+="}\n";
  out+="bool __unjoinedThreads__(){\n";
  if(Prog->branches.size()){
    for(int i = 0; i < Prog->globalVars.size(); i++){
      if(isBranch(Prog->globalVars[i]->type))
	 out += "if("+Prog->globalVars[i]->name+".__thread__.joinable()) return true;\n";
    }
  }
  out+="return false;}\n";
  out+="\n//END OF FILE";
  write();
}
bool TransProg(){
  //add trans compileing for classes
  for(vector<object *>::iterator i = Prog->classes.begin();i != Prog->classes.end();i++){
    TransClass(*i);
  }
  TransThing();
  TransRunner();
  out+="\n\n\n";
  if(Prog->globalVars.size()){
    out += "//Start of global var defs\n";
    //define global vars
    for(vector<var *>::iterator i = Prog->globalVars.begin();i != Prog->globalVars.end();i++){
      out+="/*atomic or thrad safe*/ ";
      TransVar(*i, true);
      out+=";\n";
    }
    out += "//End of global var defs\n";
  }

  for(vector<Funk *>::iterator i = Prog->functions.begin();i != Prog->functions.end();i++){
    if(!TransFunk(*i)) return false;
  }
  return true;
}
void TransClass(object * N){
  debugT("TransClass");
  out+="class "+N->name;
  if(N->isBranch){
    out+=": public runnable<"+((branch*)N)->type+">";
  }else{
    out+=": public "+N->extends;
  }
  out+="{\npublic: \n";
  
  for(vector<var *>::iterator i = N->memberVars.begin();i != N->memberVars.end();i++){//TODO organise type
      TransVar(*i, !((*i)->STATIC));
      out+=";\n";
  }
  for(vector<Funk *>::iterator i = N->constructors.begin();i != N->constructors.end();i++){
    TransFunk(*i, false);
  }
  for(vector<Funk *>::iterator i = N->memberFunks.begin();i != N->memberFunks.end();i++){
    TransFunk(*i);
  }
  out+="};\n";
  for(vector<var *>::iterator i = N->memberVars.begin();i != N->memberVars.end();i++){//init static vars
    if(((*i)->STATIC)){
      TransClassVar(*i, N->name);
      out+=";\n";
    }
  }
  //add type() for class
  out += "string type(const "+N->name+" NoN){\n\treturn \""+N->name+"\";}\n";
  debugT("TransClass");
}
void TransClassVar(var * N, string object){
  debugT("TransClassVar()");
  if(N->atomic)
    out+="atomic<";
  if(N->type == INT_NAME){
    out+="int ";
  }else if(N->type == DOUBLE_NAME){
    out+="double ";
  }else if(N->type == STRING_NAME){
    out+="string ";
  }else if(N->type == ANY_NAME){
    out+="__ANY__ ";
  }else if(N->type == VECTER_NAME){
    out+="vector<__ANY__> ";
  }else{
    out+=N->type+" ";
  }
  if(N->atomic)
    out+=">";
  if(N->reference)
    out+='&';
  if(N->ptr)
    out+='*';
  out+=" "+object+":: "+N->name;
  if(N->startingValue != NULL){
    out+= "= ";//+N->startingValue;
    TransExp(N->startingValue);
  }
  debugT("TransClassVar() done");
}
void TransVar(var * N,bool incudeStartingValue){
  debugT("TransVar()");
  if(N->constant) out+=" const ";
  if(N->STATIC) out+=" static ";
  if(N->atomic) out+= "atomic<";
  //cout<<"IsList: "<<N->isList<<endl;
  if(N->isList){
    out += varListDef(N);
  }else if(N->type == INT_NAME){
    out+="int ";
  }else if(N->type == DOUBLE_NAME){
    out+="double ";
  }else if(N->type == STRING_NAME){
    out+="string ";
  }else if(N->type == ANY_NAME){
    out+="__ANY__ ";
  }else if(N->type == VECTER_NAME){
    out+="vector<__ANY__> ";
  }else{
    out+=N->type+" ";
  }
  if(N->atomic) out+= "> ";
  if(N->reference)
    out+="& ";
  //if(!N->isList)
  out+=N->name;
  if(incudeStartingValue && N->startingValue != NULL){
    out+= "= ";//+N->startingValue;
    TransExp(N->startingValue);
  }
  if(N->secondaryType == "..."){
    out+=" ... ";
  }
  debugT("TransVar() done");
}
void TransFunkKW(Funk * N, bool regFunk, bool includeStartingValue){
  debugT("TransFunkKW()");
  if(regFunk){
    //out+=(N->STATIC?"static ":"")+N->returnType + " ";
    out+=(N->STATIC?"static ":"");
    TransVar(&N->returnType);
  }
  out+=N->name+" (";
  for(vector<var>::iterator i = N->parameters.begin();i != N->parameters.end();){
    if(i->secondaryType != "...") TransVar(&(*i), includeStartingValue);
    i++;
    if(i != N->parameters.end()){
      out+=", ";
    }
  }
  if(N->dynamicParamType != ""){//end with ... if function is ver...
    if(N->parameters.size())
      out+=", ";
    out+= "int argLen ...";
  }

  out+=")";
  debugT("TransFunkKW() done");
}

bool TransFunk(Funk * N, bool regFunk){
  debugT("TransFunk("+N->name+")");
  /*if(regFunk)
    out+=(N->STATIC?"static ":"")+N->returnType + " ";
  out+=N->name+" (";
  for(vector<var>::iterator i = N->parameters.begin();i != N->parameters.end();){
    if(i->secondaryType != "...") TransVar(&(*i),false);
    i++;
    if(i != N->parameters.end()){
      out+=", ";
    }
  }
  if(N->dynamicParamType != ""){//end with ... if function is ver...
    out+= ", int argLen ...";
  }
  */
  TransFunkKW(N, regFunk);
  out+="{\n";
  //TODO add things for ...
  if(N->dynamicParamType != ""){//really only works with __ANY__
    out += "__ANY__ " + N->dynamicParamVar + ";\n";
    out += "if(argLen){\n";
    out += "va_list ap;\nva_start(ap, argLen);\n";
    out += N->dynamicParamVar + " = list(va_arg(ap, __ANY__));\nfor(int i = 2; i <= argLen; i++){\n"+ N->dynamicParamVar +" += va_arg(ap, __ANY__);}\nva_end(ap);}";
  }//add unum support
  if(!TransBlock(N->funkBlock)){
    cout<<"Could not parse funk"<<endl;
      return false;
  }
  out+="\n}\n";
  debugT("TransFunk("+N->name+") done");
  return true;
}
bool TransBlock(Block * B){
  debugT("TransBlock()");
  if(B == NULL ||  B->Lines.size() == 0){ debugT("TransBlock()EMPTY done"); return true;}
  for(vector<parseNode*>::iterator i =  B->Lines.begin();i!=B->Lines.end();i++){
    TransLine((*i));
  }
  debugT("TransBlock() done");
  return true;
}
bool TransLine(parseNode * N){
  debugT("TransLine()");
  if(N->type == "WHILE"){
    out+="/*while loop*/";
    TransWHILE((WHILE *)N->theThing);
  }else if(N->type == "FOR_IN"){
    out+="/*FOR_IN*/";
    TransFOR_IN((FOR_IN *)N->theThing);
    /*}else if(N->type == "FOR"){
    TransFOR((FOR *)N->theThing);
  }*/
  }else if (N->type == "IF"){
    out+="/*IF*/";
    TransIF((IF *)N->theThing);
  }else if(N->type == "varAss"){
    out+="/*VarAss*/";
    TransVarAss((varAssign *)N->theThing);
  }else if(N->type == "funkCall"){
    out+="/*FunkCall*/";
    TransFunkCall((funkCall *)N->theThing);
  }else if(N->type == "exp3"){
    TransExp((expression3 *)N->theThing);
  }else if(N->type == "ret"){
    out+="return ";
    TransExp((expression3 *)N->theThing);
    //}else if(N->type == "class"){
    //TransCLASS((CLASS*)N->theThing);
  }else if(N->type == "BLOCK_COM"){
    TransBlockComment(N);
  }else if(N->type == "SINGLE_COM"){
    TransSingleComment(N);
  }else if(N->type == "FOR2"){
    TransFOR2((FOR2 *)N->theThing);
  }else if(N->type == "C++"){
    out+=N->data;
  }else if(N->type == "TRY"){
    TransTRY((tryCatch *)N->theThing);
  }else if(N->type == "throw"){
    out+= "throw runtime_error(";
    TransExp((expression3 *)N->theThing);
    out+=");";
  }else if(N->type == "EMPTY"){
    //Do nothing
  }else{
    out+="/*IDK: ";
    out+=to_string(N->ID);
    out+="*/";
    out+=N->toString();
  }
  out+=";\n";
  debugT("TransLine() done");
  return true;
}
bool TransTRY(tryCatch * N){
  debugT("TransTRY()");
  out+= "try{";
  TransBlock(N->tryBlock);
  out+="}catch(";
  //TODO exception type
  out+="exception& ";
  out+=N->exceptionName;
  out+="_exception_ttl";
  out+="){";
  out+="__ANY__ "+N->exceptionName+" = "+N->exceptionName+"_exception_ttl.what();";
  TransBlock(N->catchBlock);
  out+="};";
  debugT("TransTRY() done");
  return true;
}

bool TransFunkCall(funkCall *N){//outputs compleated function call, name & params
  debugT("TransFunkCall("+N->funkName+")");
  out+=N->funkName+"(";
  if(N->parameters.size() == 0){//TODO better way of doing this? in one loop?
    out+=")";
    debugT("TransFunkCall() done");
    return true;
  }
  int i = 0;
  for(;i<N->parameters.size()-1;){
    TransExp(N->parameters[i]);
    i++;
    out+=",";
  }
  TransExp(N->parameters.at(i));
  out+=")";
  debugT("TransFunkCall() done");
  return true;
}
bool TransVarAss(varAssign * N){
  debugT("TransVarAss()");
  TransVar(N->VAR, false);
  /*if(N->VAR->constant) out+="const ";
  out+=N->VAR->type;
  */
  if(N->exp3 == NULL){
    out += ";";//no starting value
  }else{
    out+=" "+N->OP;//+N->VAR->name+N->OP;
    TransExp(N->exp3);//Change to allow Parser one
  }
  
  debugT("TransVarAss() done");
  return true;
}
bool TransExpHelp(expression * N){
  debugT("TransExpHelp()");
  int i = 0;
  for(;i<N->atoms.size()-1;i++){
    TransAtom(N->atoms[i]);
    out+=" ";
    out+=N->OPs[i];
    out+=" ";
  }
  i++;
  TransAtom(N->atoms.at(i-1));
  debugT("TransExpHelp() done");
  return true;
}
bool TransExpHelp(expression2 * N){
  debugT("TransExpHelp2()");
  if(N->leftExp != NULL)
    if(N->leftIsExp){
      TransExp(N->leftExp);
    }else{
      TransAtom(N->leftAtom);
    }
  
  if(N->OP != ""){
    out+=' ';
    out+=N->OP;
    out+=' ';
  }
  if(N->rightExp != NULL)
    if(N->rightIsExp){
      TransExp(N->rightExp);
    }else{
      TransAtom(N->rightAtom);
    }
  
  debugT("TransExpHelp2() done");
  return true;
}

bool TransExp(expression2 * N){
  debugT("TransExp2()");
  //out+="(";
  if(N->cast ==""){
    TransExpHelp(N);
  }else if(N->cast == "__ANY__"){
    out+="__ANY__(";
    TransExpHelp(N);
    out+=")";
  }else if(N->cast == "num"|| N->cast =="double"){
    out+="(";
    TransExpHelp(N);
    out+=").toNum()";
  }else if(N->cast == "string"){
    out+="(";
    TransExpHelp(N);
    out+=").toString()";//?
  }else if(N->cast == "int"){
    out+="(int)(";
    TransExpHelp(N);
    out+=").toNum()";
  }else if(N->cast == "string to num"){
    out+="(";
    TransExpHelp(N);
    out+=").length()";
  }else if(N->cast == "num to string"){
    out+="::toString(";
    TransExpHelp(N);
    out+=")";
  }else if(N->cast == "bool"){
    out+="(bool)(";
    TransExpHelp(N);
    out+=").toNum()";
  }else{
    report("Unknown type cast(exp2 trans): "+N->cast, 0);
    /*cout<<(N->cast).size()<<endl;
    cout<<string("__ANY__").size()<<endl;
    cout<<N->cast<<endl;
    for(int i = 0; i < (N->cast).size();i++)
      cout<<(int)(N->cast)[i]<<endl;
      throw 99;*/
    out += "*("+N->cast+"*)((";
    TransExpHelp(N);
    out += ").ptr)";
  }
  debugT("TransExp2() done");
  return true;
}


void TransExpHelp(expression3 * N){
  debugT("TransExpHelp3()");
  for(vector<bigAtom*>::iterator itt= N->bigAtoms->begin();itt != N->bigAtoms->end();itt++){
    if((*itt)->type == bigAtom::ATOM){
      TransAtom((*itt)->a);
    }else if((*itt)->type == bigAtom::EXP3){
      TransExp((*itt)->exp3);
    }else if((*itt)->type == bigAtom::EXP2){
      TransExp((*itt)->exp2);
    }else{
      out+=(*itt)->op;
    }
    out+=' ';
  }
  debugT("TransExpHelp3() done");
}

void TransExp(expression3 * N){
  debugT("TransExp3()");
  if(N->ptr)
    out+="&(";
  if(N->cast ==""){
    TransExpHelp(N);
  }else if(N->cast == "__ANY__"){
    out+="__ANY__(";
    TransExpHelp(N);
    out+=")";
  }else if(N->cast == "num"|| N->cast =="double"){
    out+="(";
    TransExpHelp(N);
    out+=").toNum()";
  }else if(N->cast == "string"){
    out+="string(";
    TransExpHelp(N);
    out+=")";
  }else if(N->cast == "int"){
    out+="(int)(";
    TransExpHelp(N);
    out+=").toNum()";
  }else if(N->cast == "length"){
    TransExpHelp(N);
    out+=".length()";
  }else if(N->cast == "string_length"){
    out+="string(";
    TransExpHelp(N);
    out+=").length()";
  }else if(N->cast == "any to string"){
    out+="(";
    TransExpHelp(N);
    out+=").toString()";
  }else if(N->cast == "num to string"){
    out += "::toString(";
    TransExpHelp(N);
    out+=")";
  }else if(N->cast == "any_num"){
    out+="__ANY__(";
    TransExpHelp(N);
    out+=").toNum()";
  }else if(N->cast == "list"){
    out+="{";
    TransExpHelp(N);
    out+="}";
  }else if (N->cast == "ptr"){
    out+= "&(";
    TransExpHelp(N);
    out+=")";
  }else if(N->cast == "new"){
    out+= "new (";
    TransExpHelp(N);
    out+=")";
  }else{
    TransExpHelp(N);
  }
  if(N->ptr)
    out+=")";
  debugT("TransExp3() done");
}

bool TransAtomHelp(atom * N){
  debugT("TransAtomHelp("+N->type+")");
  //out+=N->front;
  if(N->type == "lit"){
    //cout<<"Lit is: "<<*(N->literalValue)<<endl;
    if(N->stringLit)
      out+="string(";
    out+=(*(N->literalValue));
    if(N->stringLit)
      out+=")";
  }else if(N->type == "funkCall"){
    TransFunkCall(N->fCall);
  }else if(N->type == "[]exp3"){
    out+="[";
    TransExp(N->exp3);
    out+="]";
  }else if(N->type == "[]exp2"){
    out+="[";
    TransExp(N->exp2);
    out+="]";
  }else if(N->type == "exp3"){
    if(N->exp3->bigAtoms->size() > 1)
      if(N->exp3->cast != "list") out+="(";//poor way of doing this
    TransExp(N->exp3);
    if(N->exp3->bigAtoms->size() > 1)
      if(N->exp3->cast != "list") out+=")";
  }else if(N->type == "exp2"){
    if(N->exp2->cast != "list") out+="(";
    TransExp(N->exp2);
    if(N->exp2->cast != "list") out+=")";
  }else if(N->type == "objCall"){
    //out+=N->base+".";
    //TransFunkCall(N->fCall);//should funcCall itself have base?
  }else{
    cout<<"TransAtom idk what type: "<<N->type<<endl;
    return false;
  }
  //out+=N->end;
  debugT("TransAtomHelp() done");
  return true;
}
bool TransAtom(atom * N){
  debugT("TransAtom()");
  //cout<<"cast is: "<<N->cast<<endl;
  //cout<<"data: "<<*(N->literalValue)<<endl;
  if(N->cast == "__ANY__"){
    out+="__ANY__(";
    TransAtomHelp(N);
    out+=")";
  }else if(N->cast == "num" || N->cast =="double"){
    out+="(";
    TransAtomHelp(N);
    out+=").toNum()";
  }else if(N->cast == "string"){
    out+="string(";
    TransAtomHelp(N);
    out+=")";
  }else if(N->cast == "string to num"){
    out+="(";
    TransAtomHelp(N);
    out+=").length()";
  }else if(N->cast == "num to string"){
    out+="::toString(";
    TransAtomHelp(N);
    out+=")";
  }else if(N->cast == "any string"){
    out+="(";
    TransAtomHelp(N);
    out+=").toString()";
  }else{
    TransAtomHelp(N);
  }
  debugT("TransAtom() done");
  return true;
}
bool TransBoolState(boolStatment * N){//TODO get rid of
  debugT("TransBoolState()");
  int i = 0;
  //cout<<"size: "<<N->atoms.size()<<endl;
  for(;i<N->exps.size()-1;i++){
    //cout<<i<<endl;
    //TransExp(N->exps[i]);
    out+=' ';
    out+=N->OPs[i];
    out+=' ';
  }
  //cout<<"last one"<<endl;
  i++;
  //TransExp(N->exps.at(i-1));
  debugT("TransBoolState() done");
  return true;
}
bool TransIF(IF * N){
  debugT("TransIF()");
  out+="if(";
  if(N->usingBool){
    TransBoolState(N->boolStatments->front());
  }else{
    TransExp(N->exps3->front());
  }
  out+="){\n";
  TransBlock(N->blockStatments->front());
  out+="}";
  if(N->blockStatments->size() == 1) return true;
  out+='\n';
  int x = 0;
  if(N->usingBool){
    for(vector<boolStatment *>::iterator i = N->boolStatments->begin()+1;i != N->boolStatments->end();i++){
      out+="else if(";
      TransBoolState(*i);
      out+="){\n";
      x++;
      TransBlock(N->blockStatments->at(x));
      out+="}";
    }
  }else{
    for(vector<expression3 *>::iterator i = N->exps3->begin()+1;i != N->exps3->end();i++){
      out+="else if(";
      TransExp(*i);
      out+="){\n";
      x++;
      TransBlock(N->blockStatments->at(x));
      out+="}";
    }
  }
  if(N->blockStatments->size() == N->boolStatments->size()){out+="/*end IF*/ \n"; return true;}
  out+="else{\n";
  TransBlock(N->blockStatments->back());
  out+="}/*end of IF*/";
  debugT("TransIF() done");
  return true;
}
bool TransWHILE(WHILE * N){
  if(N->isNorm){
    out+="while(";
    TransExp(N->exp3);
    out+="){\n";
    TransBlock(N->blockState);
    out+="}//end WHILE\n";
  }else{
    out+="do{";
    TransBlock(N->blockState);
    out+="}while(";
    TransExp(N->exp3);
    out+=");\n";
  }
  return true;
}
void TransFOR2(FOR2 * N){
  debugT("TransFOR2()");
  out+="/*FOR2*/\n";
  if(N->for_in){
    if(N->isNum){
      out+="for(";
      TransVar(N->ittName);//should only be name
      if(N->start3 == NULL){
	out+= "=0";
      }else{
	out+="= ";
	TransExp(N->start3);
      }
      out+=";";
      TransVar(N->ittName);//should only be name
      out+="!=";
      TransExp(N->end3);
      out+=";";
      TransVar(N->ittName);//should only be name
      out+="++){";
    }else{
      currentIttNum++;//used for keeping track of for loop itterators, so no dup var names
      string itt = "for_itt_"+to_string(currentIttNum);
      out+="int "+itt;
      if(N->start3 != NULL){
       	out+=" = toInt(";
	//TODO get rid of cast?
	N->start3->cast = "";
	TransExp(N->start3);
	out+=");";
      }else{
	out+="=0;";
      }
      string end = "for_end_"+to_string(currentIttNum);
      out+="__ANY__ "+end+"=";
      TransExp(N->end3);
      out+=";\n";
      out+="for(__ANY__ "+N->ittName->name+";"+itt+"<(int)"+end+".toNum()"+";"+itt+"++){\n";//TODO < or <= or != or <?
      out+="if("+end+".type == \"_V\" || "+end+".type == \"_S\"){\n";
      out+=N->ittName->name+"="+end+"["+itt+"];\n";
      out+="}else{\n"+N->ittName->name+"="+itt+";\n";
      out+="}\n";
    }
  }else{//normal for (for ;;)
    out+="for(";
    TransLine(N->lineOne);
    TransExp(N->lineTwo3);
    out+=";";
    TransLine(N->lineThree);
    //remove trailing "; " from TransLine
    out.pop_back();
    out.pop_back();
    out+="){";
  }
  TransBlock(N->blockExpression);
  out+="}/*end of FOR2*/";
  debugT("TransFOR2() done");
}
bool TransFOR_IN(FOR_IN * N){
  debugT("TransFOR_IN()");
  if(N->vectorRange){
    string vName = N->itteratorName+"_IT";//TODO i would actually like to be able to do this with not just vectors, but also for in to other types
    //would also avoid error as you can for in to non vectors. should use [] oporator
    out+="for(vector<__ANY__>::iterator "+vName+" = ((vector<__ANY__>*)"+N->varName;
    out+=".ptr)->begin(); "+vName+" != ((vector<__ANY__>*)"+N->varName+".ptr)->end(); "+vName+"++){__ANY__ "+N->itteratorName+"=* "+vName+";";
  }else{out+="for(int "+N->itteratorName+" = "+N->start+"; "+N->itteratorName+" != "+N->end+"); "+N->itteratorName+"++){";}
  TransBlock(N->blockState);
  out+="}/*end FOR_IN*/\n";
  debugT("TransFOR_IN() done");
  return true;
}
bool TransSingleComment(parseNode * N){
  if(N->data!="")
    out+="//"+N->data;
  return true;
}
bool TransBlockComment(parseNode * N){
  if(N->data!="")
    //out+="/*"+N->data+"*/ \n";
    out+=N->data+" \n";
  return true;
}
void TransImport(){
  /*
    out+="#include "+N->importFile+".h\n";
    if(N->knownAs != "")
    return;
    out+="#define "+N->importFile +" "+N->knownAs+";\n";
   */
}
//TODO use std:array not array
const string varListDef(var * v){
  debugT("varListDef()");
  string ret = "", end = "";;
  if(v->arr.size()){
    unsigned len = 0;
    for(;len<v->arr.size();len++){
      if(v->arr[len] == 0){
	ret+="vector<";
	end = ">" + end;
      }else{
	ret += "array<";
	end = ","+to_string(v->arr[len]) + ">" + end;
      }
    }
    ret+=v->type;
  }
  debugT("varListDef() done");
  return ret + end + " ";// + v->name;
}
string testMe(vector<unsigned long> cx){
  var * a = new var();
  a->type = "string";
  a->arr = cx;
  a->name = "testVar";
  return varListDef(a);
}
void TransBranch(branch * B){
  out+="class "+B->name+": public runnable<"+B->type+">{\n";
  out+="public:\n";
  for(int i = 0; i<B->memberVars.size(); i++)
    TransVar(B->memberVars.at(i));
  for(int i = 0; i<B->memberFunks.size(); i++)
    TransFunk(B->memberFunks.at(i));
  
}

const string resolveVarType(var * v){
  // returns type of var as string
  string tmp = "";

  for(int i = 0; i < v->arr.size(); i++){
    tmp+="[";
    if(v->arr[i] > 0)
      tmp+= to_string(v->arr[i]);
    tmp+="]";
  }
  
  return tmp;//TODO
}

void write(){
  //appendFile(outFileName.c_str(),&out);
  appendFile(cppFileName.c_str(),&out);
  out = "";
}
void debugT(const string& s){
  if(reportingLevel > 0) return;
  if(s.at(s.length()-1) == ')')
    indentT++;//call
  string me = "";
  //cout<<"123"<<endl;
  for(int i = 0; i<=indentT; i++)
    me+=" ";
  //me+=s+": "+sym->tokenText;
  me+=s;
  //cout<<"A"<<endl;
  report(me,-2);
  if(s.at(s.length()-1) == 'e')
    indentT--;//return(done)
}

