
#include "Interp.h"
#include "_interpScope.h"

scope parseVars;// = scope();//something, init
interpMe vars;

int Interp(){
  int exitStatus = 0;
  interpMode = true;
  printWelcome();//these lines could go in setup() or init()
  Prog = new prog;
  setUpUserMain(Prog);
  parseVars = scope();
  //TODO add vars to parseVars
  vars = interpMe();
  vars.setUpVars();
  srand(time(NULL) + randInt());
  stackTrace = vector<string>();
  stackTrace.push_back("userMain");
  bool cont = true;
  do{
    indent = 0;
    implicitPrint = false;
    if(reportingLevel < 1){cout<<"peeking now"<<endl;vars.peekScope(); analizeProg(Prog);}
    string data = getInput();
    int flagStatus = checkFlags(data);//checks flags like exit and help; returns 1 if exit, 2 if help/about, 0 if no flag
    if(flagStatus == 1)
      //return 0;
      break;
    if(!flagStatus){
      try{
	exitStatus = executeInterp(data);
      }catch(returnExit e){
	exitStatus = e.exitCode;
	cont = false;
      }
    }
  }while(cont);
  delete Prog;
  vars.cleanMeOrElseIllLeak();//not sure about this?
  report("Exited with code: "+to_string((long long) exitStatus), 0);
  return exitStatus;
}
int executeInterp(string data){//Make string ptr
  vector<TokenData> tokens;
  if(!Lexer(&data,&tokens)){
    cout<<"Unable to lex"<<endl;//print error msg
    return false;
  }
  if(reportingLevel < 0){
    cout<<"Symbols: "<<tokens.size()<<endl;
    for(vector<TokenData>::iterator i = tokens.begin(); i != tokens.end(); i++){
      report(TokenData::getTokenTypeName(i->tokenType)+": "+i->tokenText+": at "+to_string((long long)i->charPos)+": "+to_string((long long)i->line),-1);
    }
    report("\n\n--------------------------\n\n",-2);
  }
  //need to set parseVars
  if(!ParseTwo(&tokens, &parseVars, Prog, true) || errors){
    report("Could not parse input, "+to_string((long long) errors) +" error"+(errors == 1?"":"s")+" found", 2);
    errors = 0;//reset errors
    cleanUp(Prog);
    return false;
  }

  try{
    interpProg(Prog);
  }catch(returnExit e){//exits interpreter
    throw e;
  }catch(returnExcep& e){//return value
    print(e.data);
  }catch(exception& e){//TODO this doesnt print the proper error msg
    cout<<"Error interpreting: "<<e.what()<<endl;
  }
  cleanUp(Prog); //we want to run, then delete Main;  keep all other functions
  return false;
}
void cleanUp(prog * Prog){
  report("Cleaning up",-2);
  for(int i = 0; i<Prog->functions.size();i++){
    if(Prog->functions.at(i)->name == "__userMain__"){
      report("Found main for cleaning at location: "+to_string((long long) i), -2);
      for(auto messyPtr : Prog->functions.at(i)->funkBlock->Lines){
	delete messyPtr;
      }
      Prog->functions.at(i)->funkBlock->Lines.clear();
      //delete Prog->functions.at(i)->funkBlock;
      //Prog->functions.at(i)->funkBlock = new Block();
    }
  }
  report("All clean", -2);
}
int checkFlags(const string& data){//or check for exit(), 0= no flags, 1=exit, 2 = other flag(dont execute but dont exit)
  if(data == string("exit\n\n")){
    return 1;
  }
  if(data == string("help\n\n")){
    interpHelp("");
    return 2;
  }
  if(data == string("dump\n\n")){
    vars.peekScope();
    return 2;
  }
  return 0;
}
void printWelcome(){cout<<"\nWelcome to Twine interpreter "<<versionNumber<<"\nEnter help for help or exit to exit the interpreter\n";}
string getInput(){//TODO use readline
  string lines = "";
  cout<<"\n>";
  int per = 0;
  do{
    string line = "\n";
    getline(cin, line);
    lines+=line+'\n';
    for(int i = 0; i<line.length();i++){//check this as reading in line?
      if(line.at(i) == '{'){//I know this doesnt do comments
	per++;
      }else if(line.at(i) == '}'){
	per--;
      }
    }
  }while(per>0);
  //lines+="//end of user input\n";
  return lines+'\n';
}

void initialVarSetUp(){
  setVar("args","__ANY__");//TODO
  for(int i =0;i<sizeof(builtInValues)/sizeof(builtInValues[0]);i++){
    setVar(builtInValues[i].name,builtInValues[i].type);
  }
}

bool interpProg(prog * Prog){//since it parsed already, it should run okay
  //should i just let Prog be global so its easy for everyone to use? Same with scope?
  debug("interpProg()");
  //TODO import externals

  try{
    //Execute main
    interpBlock((*Prog->functions.begin())->funkBlock, false);//TODO what about interpUserFunkCall to main?
    
    //funkCall fc = funkCall();
    //interpUserFunkCall((*Prog->functions.begin()), &fc);
    
  }catch(returnExit e){
    debug("interpProg(EXIT) done");
    throw e;
  }catch(returnExcep e){
    debug("interpProg(RETURN VALUE) done");
    throw e;
  }catch(exception& e){
    debug(string("interpProg(ERROR EXCEPTION: ")+e.what()+") done");
    throw e;
  }
  
  debug("interpProg() done");
  return true;
}
__ANY__ interpUserFunkCall(Funk * f, funkCall * fc){
  //TODO add to stack trace
  debug("interpFunk("+f->name+")");
  vars.pushScope();
  //set func params
  __ANY__ data = false;
  for(int i = 0; i < f->parameters.size(); i++){
    if(i >= fc->parameters.size()){//set var as defult
      data = interpExpression(f->parameters.at(i).startingValue);
    }else{//set var as passed in value
      data = interpExpression(fc->parameters.at(i));
    }
    vars.setVar(f->parameters.at(i).name, data, false, f->parameters.at(i).type);
  }
  __ANY__ returnData = false;
  try{
    interpBlock(f->funkBlock, false);
  }catch(returnExcep e){
    report("Got return value ", -2);
    returnData = e.data;
  }
  vars.pullScope();
  debug("interpFunk("+f->name+") done");
  return returnData;
}
void interpBlock(Block * b, bool pushScope){
  debug("interpBlock()");
  if(pushScope)
    vars.pushScope();
  try{
    for(vector<parseNode*>::iterator it = b->Lines.begin();it!=b->Lines.end();it++){
      interpLine((*it));
    }
  }catch(returnExit e){
    if(pushScope)
      vars.pullScope();
    debug("interpBlock(EXIT) done");
    throw e;
  }catch(returnExcep e){
    if(pushScope)
      vars.pullScope();
    debug("interpBlock(RETURN VALUE) done");
    throw e;
  }catch(exception& e){
    if(pushScope)
      vars.pullScope();
    debug(string("interpBlock(ERROR EXCEPTION: ")+e.what()+") done");
    throw e;
  }
  debug("interpBlock() done");
}

void interpLine(parseNode * N){
  debug("interpLine()");
  if(N->type == "WHILE"){
    interpWHILE((WHILE *)N->theThing);
  }else if(N->type == "FOR2"){
    interpFOR2((FOR2 *)N->theThing);
  }else if (N->type == "IF"){
    interpIF((IF *)N->theThing);
  }else if(N->type == "varAss"){
    interpVarAss((varAssign *)N->theThing);
  }else if(N->type == "funkCall"){
    interpFunkCall((funkCall *)N->theThing);
  }else if(N->type == "exp3"){//TODO
    if(implicitPrint)
      print(interpExpression((expression3 *)N->theThing));
  }else if(N->type == "exp2"){//TODO
    if(implicitPrint)
      print(interpExpression((expression2 *)N->theThing));
  }else if(N->type == "ret"){
    returnExcep ret = returnExcep();
    ret.data = interpExpression((expression3 *)(N->theThing));
    debug("interpLine(RETURNING VALUE) done");
    throw ret;
  }else if(N->type == "TRY"){
    interpTry((tryCatch *)N->theThing);
  }else if(N->type == "throw"){
    throw runtime_error(interpExpression((expression3 *)N->theThing).toString());
  }else{
    cout<<"TODO: "<<N->type<<endl;
  }
  debug("interpLine() done");
}
void interpFOR2(FOR2 * f){
  debug("interpFOR2()");
  if(f->for_in){
    if(f->isNum){

      vars.pushScope();
      //set var itterator
      
      vars.setVar(f->ittName->name, interpExpression(f->start3));
      //while
      while(vars.getVar(f->ittName->name) != interpExpression(f->end3)){//TODO save exp and not re-check
      //set var
	vars.setVar(f->ittName->name, vars.getVar(f->ittName->name).toNum() + 1);
      //do block
	interpBlock(f->blockExpression);
      }
      vars.pullScope();
    }else{
      vars.pushScope();
      int startingValue = 0;
      if(f->start3 != NULL){
	startingValue = interpExpression(f->start3).toNum();
      }
      __ANY__ tmp = interpExpression(f->end3);
      while(startingValue < (int)interpExpression(f->end3).toNum()){
	if(tmp.type == "_V" || tmp.type == "_S"){
	  vars.setVar(f->ittName->name, tmp[startingValue]);
	}else{
	  vars.setVar(f->ittName->name, __ANY__(startingValue));
	}
	interpBlock(f->blockExpression, false);
	startingValue++;
      }
      vars.pullScope();
    }
  }else{
  }
  debug("interpFOR2() done");
}
void interpIF(IF * f){
  debug("interpIF()");
  int i = 0;
  for(; i < f->exps3->size();i++){
    if(interpExpression(f->exps3->at(i)).toNum()){
      interpBlock(f->blockStatments->at(i));
      debug("interpIF() done");
      return;
    }
  }
  if(i<f->blockStatments->size()){//else
    interpBlock(f->blockStatments->back());
  }
  debug("interpIF() done");
}
void interpVarAss(varAssign * va){
  debug("interpVarAss()");
  //va has type, name, and exp(it actually has two, not sure why)
  if(va->OP == "="){
    //cout<<"New var?: "<<va->newVar<<endl;
    if(va->newVar){
      vars.newVar(va->VAR->name, interpExpression(va->exp3), false, va->VAR->type);
    }else{
      vars.setVar(va->VAR->name, interpExpression(va->exp3), false, va->VAR->type);
    }
    /*TODO*/ //setVar(va->VAR->name, va->VAR->type, false, false);
  }else if(va->OP == "+="){
    vars.setVar(va->VAR->name, vars.getVar(va->VAR->name)+interpExpression(va->exp3), false, va->VAR->type);
  }else if(va->OP == "-="){
    vars.setVar(va->VAR->name, vars.getVar(va->VAR->name)-interpExpression(va->exp3), false, va->VAR->type);
  }else if(va->OP == "*="){
    vars.setVar(va->VAR->name, vars.getVar(va->VAR->name)*interpExpression(va->exp3), false, va->VAR->type);
  }else if(va->OP == "/="){
    vars.setVar(va->VAR->name, vars.getVar(va->VAR->name)/interpExpression(va->exp3), false, va->VAR->type);
  }else if(va->OP == "%="){
    vars.setVar(va->VAR->name, vars.getVar(va->VAR->name)%interpExpression(va->exp3), false, va->VAR->type);
  }else{
    throw invalid_argument("Not implimented, var assign ");//TODO, apply op to var, then set it to new value
  }
  debug("interpVarAss() done");
}
void interpWHILE(WHILE * n){
  debug("interpWHILE()");
  while(interpExpression(n->exp3).toNum()){
    interpBlock(n->blockState); 
  }
  debug("interpWHILE() done");
}

__ANY__ interpFunkCall(funkCall * fc){ 
  debug("Call to function: "+fc->funkName+" in interpFunkCall()--");
  //TODO search user funks in prog first
  for(int i = 0; i<Prog->functions.size();i++){//TODO free memory
    if(Prog->functions.at(i)->name == fc->funkName){
      stackTrace.push_back(fc->funkName);
      __ANY__ data = interpUserFunkCall(Prog->functions.at(i), fc);
      debug("Call to function: "+fc->funkName+" in interpFunkCall() done--");
      stackTrace.pop_back();
      return data;
    }
  }
  //else
  report("Looking through hard coded funcs", -2);
  if(fc->funkName == "print"){
    //TODO how can we do these faster?
    print(interpExpression(fc->parameters.at(0)).toString(), interpExpression(fc->parameters.at(1)).toString(), interpExpression(fc->parameters.at(2)).toString());
  }else if(fc->funkName == "printn"){
    printn(interpExpression(fc->parameters.at(0)).toString(), interpExpression(fc->parameters.at(1)).toString(), interpExpression(fc->parameters.at(2)).toString());
  }else if(fc->funkName == "input"){
    return input(interpExpression(fc->parameters.at(0)).toString());
  }else if(fc->funkName == "len"){
    return len(interpExpression(fc->parameters.at(0)));
  }else if(fc->funkName == "min"){
    return min(interpExpression(fc->parameters.at(0)));
  }else if(fc->funkName == "max"){
    return max(interpExpression(fc->parameters.at(0)));
  }else if(fc->funkName == "randInt"){
    return randInt(interpExpression(fc->parameters.at(0)).toNum(),interpExpression(fc->parameters.at(1)).toNum());//TODO how to handle funs withdefualt params? make parse add in defualts?
  }else if(fc->funkName == "randDouble"){
    return randDouble(interpExpression(fc->parameters.at(0)).toNum(),interpExpression(fc->parameters.at(1)).toNum());
  }else if(fc->funkName == "randStr"){
    return randStr(interpExpression(fc->parameters.at(0)).toNum());//TODO add new param?
  }else if(fc->funkName == "round"){
    return round(interpExpression(fc->parameters.at(0)).toNum());
  }else if(fc->funkName == "read"){
    return read(interpExpression(fc->parameters.at(0)).toString());
  }else if(fc->funkName == "write"){
    return write(interpExpression(fc->parameters.at(0)).toString(),interpExpression(fc->parameters.at(1)).toString(),interpExpression(fc->parameters.at(2)).toNum());
  }else if(fc->funkName == "sleep"){
    sleep(interpExpression(fc->parameters.at(0)).toNum());
    return false;//false is defualt reutrn for all
  }else if(fc->funkName == "sort"){
    return sort(interpExpression(fc->parameters.at(0)));
  }else if(fc->funkName == "sqrt"){
    return sqrt(interpExpression(fc->parameters.at(0)).toNum());
  }else if(fc->funkName == "clear"){
    clear();
  }else if(fc->funkName == "stack"){
    return getStackTraceAsANY();
  }else if(fc->funkName == "system"){
    return system(interpExpression(fc->parameters.at(0)).toString().c_str());
  }else if(fc->funkName == "getenv"){
    return getenv(interpExpression(fc->parameters.at(0)).toString().c_str());
  }else if(fc->funkName == "toInt"){//is still an __ANY__ but is not underlying int
    return toInt(interpExpression(fc->parameters.at(0)));
  }else if(fc->funkName == "type"){
    return type(interpExpression(fc->parameters.at(0)));
  }else if(fc->funkName == "setReport"){
    reportingLevel = (int)interpExpression(fc->parameters.at(0)).toNum();
    return 0;
  }else if(fc->funkName == "toDouble"){
    return toDouble(interpExpression(fc->parameters.at(0)));
    //list
  }else if(fc->funkName == "list"){
    return list(interpExpression(fc->parameters.at(0)));
  }else if(fc->funkName == "listVars"){
    vars.peekScope();
  }else if(fc->funkName == "dropFunction"){
    //TODO
  }else if(fc->funkName == "dropVar"){
    //TODO
    /*
    try{
      removeVar(fc->parameters.at(0).toString());//remove for scope
//remove for _interpScope()
//coud just use Scope for all, use starting value as data()?
    }catch(exception e){
      cout<<"Could not remove var: "+e.what()<<endl;
      }*/
  }else if(fc->funkName == "dropClass"){
    //TODO
  }else if(fc->funkName == "listFuncs"){
    string me = "";
    for(int i = 0; i<Prog->functions.size();i++){
      me+=Prog->functions.at(i)->toString();
      me+="-----------------------------------------\n";
    }
    print(me);
    //return me;
  }else if(fc->funkName == "help"){
    interpHelp(interpExpression(fc->parameters.at(0)).toString());
    return 0;
  }else if(fc->funkName == "exit"){
    returnExit exit = returnExit();
    exit.exitCode = (int) interpExpression(fc->parameters.at(0)).toNum();
    report("Throwing exit", -2);
    throw exit;
  }else{//TODO finish all functions. is there an easier way to do this?
    cout<<"Not implimented or invalid funk call"<<endl;
  }
  //((vector<__ANY__> *)stackTrace.ptr)->pop_back();
  debug("Call to function: "+fc->funkName+" in interpFunkCall() done--");
  return __ANY__(0);
}

__ANY__ interpExpression(expression3 * exp){
  debug("interpExpression(3) with size: "+to_string(exp->bigAtoms->size())+".  ");
   //if length =1 return
   if(exp->bigAtoms->size() == 1)
     return interpAtom(exp->bigAtoms->front()->a);
   //if length =2 return apply OP
   if(exp->bigAtoms->size() == 2){
     if(exp->bigAtoms->front()->type == bigAtom::ATOM){
       return applyOP(interpAtom(exp->bigAtoms->front()->a),exp->bigAtoms->at(1)->op);
     }else{
       return applyOP(exp->bigAtoms->front()->op,interpAtom(exp->bigAtoms->at(1)->a));
     }
   }

   if(exp->cast == "list"){//parse List
     vector<__ANY__> ret = vector<__ANY__>();
     for(unsigned i = 0; i<exp->bigAtoms->size(); i++){//loop through list, i=0 is just '{'
       if(i%2 == 1){//do I need checks? should be formatted right from parser
	 if(exp->bigAtoms->at(i)->type != bigAtom::OP || exp->bigAtoms->at(i)->op != ",")
	   error("Malformed list. Expected ',', instead found: "+exp->bigAtoms->at(i)->toString());//error
       }else if(i%2 == 0 && exp->bigAtoms->at(i)->type == bigAtom::ATOM){
	 ret.emplace_back(interpAtom((exp->bigAtoms->at(i)->a)));
       }else{
	 //error
	 error("Malformed list. Expected element, instead found: "+exp->bigAtoms->at(i)->toString());
       }
     }
     return __ANY__(ret);
   }
   //find highest priority OP
   int highestPriority=-124, position = 0;
   for(int i = 0; i<exp->bigAtoms->size();i++){
     if(exp->bigAtoms->at(i)->type == bigAtom::OP && getOP(exp->bigAtoms->at(i)->op).priority>highestPriority){
       highestPriority=getOP(exp->bigAtoms->at(i)->op).priority;
       position = i;
     }
   }
   //split at there
   expression3 * left = new expression3();
   expression3 * right = new expression3();
   for(int i = 0; i<exp->bigAtoms->size();i++){
     if(i<position){
       left->bigAtoms->push_back(exp->bigAtoms->at(i));
     }else if(i>position){
       right->bigAtoms->push_back(exp->bigAtoms->at(i));
     }
   }
   //return apply op left OP right with recursion
   return applyOP(interpExpression(left), exp->bigAtoms->at(position)->op, interpExpression(right));
   //debug("interpExpression(3) done");
}

__ANY__ interpExpression(expression2 * exp){
  debug("interpExpression(2)");
  /*if(exp->right != NULL){
    return applyOP(interpAtom(exp->left),exp->OP,interpAtom(exp->right));
  }else{
    return applyOP(interpAtom(exp->left),exp->OP);
    }*/
  if(exp->OP == "."){
    __ANY__ left = interpAtom(exp->leftAtom);
    __ANY__ tmp = interpDot(left, exp->rightAtom);
    debug("interpExpression() done");
    return tmp;
  }
  __ANY__ left, right, tmp;
  if(exp->leftExp)
    if(exp->leftIsExp){
      left = interpExpression(exp->leftExp);
    }else{
      left = interpAtom(exp->leftAtom);
    }
  if(exp->rightExp)
    if(exp->rightIsExp){
      right = interpExpression(exp->rightExp);
    }else{
      right = interpAtom(exp->rightAtom);
    }
  if(exp->leftExp && exp->rightExp){
    tmp = applyOP(left, exp->OP, right);
  }else if(exp->leftExp){
    tmp = left;
  }else{
    tmp = right;
  }
  if(exp->cast == "num" || exp->cast == "string to num"){
    tmp = tmp.toNum();//string to nom, should it do proper to_double(string) convertion?exp->cast == "num to ,
  }else if(exp->cast == "string"){
    tmp = tmp.toString();
  }else if(exp->cast == "num to string"){
    tmp = toString(tmp.toNum());
  }else if(exp->cast == "num to int"){
    tmp = (int) tmp.toNum();
  }else if(exp->cast == "string to int"){
    tmp = stoi(tmp.toString());
  }else if(exp->cast == "string to double"){
    tmp = stod(tmp.toString());
  }
  debug("interpExpression(2) done");
  return tmp;
}

__ANY__ interpExpression(expression * exp){
  __ANY__ a = interpAtom(exp->atoms[0]);
  for(int i = 1;i<exp->atoms.size();i++){
    a = applyOP(a,exp->OPs[i-1],interpAtom(exp->atoms[i]));
  }
  return a;
}
__ANY__ interpAtom(atom * a){
  debug("interpAtom("+a->type+")--");
  if(a->type == "lit"){//TODO will print strings with same name as var as var vale: a = 99, print("a") prints 99, not a
    string name = *(a->literalValue);
    //cout<<"A"<<endl;
    __ANY__ c = vars.getVar(name);//assume its a var
    //cout<<"B"<<endl;
    if(c != __ANY__("DNE")){//if the value if found, return it
      //cout<<"C"<<endl;
      return c;
    }
    //cout<<"D"<<endl;
    if(a->helper.type == "num" || a->helper.type == ""){//if it is supposed to be a number, try to turn it into one
      //cout<<"E"<<endl;
      try{return __ANY__(stoi(name));}catch(invalid_argument e){}//try int
      //cout<<"E_1"<<endl;
      try{return __ANY__(stod(name));}catch(invalid_argument e){}//try double

      if(name == "true")
	return true;
      if(name == "false")
	return false;
    }
    //cout<<"F"<<endl;
    report("Returning stright lit: "+name, -2);
    __ANY__ me = __ANY__(name);
    return me;//make string
  }else if(a->type == "exp3"){
    return interpExpression(a->exp3);
  }else if(a->type == "exp2"){
    return interpExpression(a->exp2);
  }else if(a->type == "funkCall"){
    return interpFunkCall(a->fCall);
  }else{
    throw runtime_error(string("Unknown atom type: ")+a->type);
  }
  return false;
}
__ANY__ applyOP(const string& OP, __ANY__ right){
  if(OP == "++"){
    return right++;
  }else if(OP == "--"){
    return right--;
  }else{
    error("Unsupported op: "+OP);
  }
  return __ANY__(0);
}
__ANY__ applyOP(__ANY__ left, const string& OP){
  if(OP == "++"){
    return ++left;
  }else if(OP == "--"){
    return --left;
  }else{
    error("Unsupported op: "+OP);
  }
  return __ANY__(0);
}
__ANY__ applyOP(__ANY__ left, const string& OP, __ANY__ right){
  if(OP == "+"){
    return left+right;
  }else if(OP == "-"){
    return left - right;
  }else if(OP == "*"){
    return left * right;
  }else if(OP == "/"){
    return left / right;
  }else if(OP == "%"){
    return left % right;
  }else if(OP == "^"){
    return left ^ right;
  }else if(OP == "["){
    return left[right];
  }else if(OP == "<"){
    return left < right;
  }else if(OP == ">"){
    return left > right;
  }else if(OP == "<="){
    return left <= right;
  }else if(OP == ">="){
    return left >= right;
  }else if(OP == "=="){
    return left == right;
  }else if(OP == "&&"||OP=="and"){
    return left.toNum() && right.toNum();
  }else if(OP == "||"||OP=="or"){
    return left.toNum() || right.toNum();
  }else{//TODO +=,-=, etc. xor 
    error("Unsupported op: "+OP);
  }
  return __ANY__(0);
}
    
__ANY__ applyCast(__ANY__ data, const string& cast){
  //TODO check if data is already type
  if(cast == "")
    return data;
  if(data.getType() == cast)
    return data;
  if(cast == "int"){
    return (int) data.toNum();
  }else if(cast == "double" || cast == "num"){
    return data.toNum();
  }else if(cast == "string"){
    return data.toString();
  }
  return data;
}

void interpTry(tryCatch * TC){
  try{
    interpBlock(TC->tryBlock);
  }catch(returnExit e){//ignor exits and returns, only catch other exceptions
    throw e;
  }catch(returnExcep e){
    throw e;
  }catch(exception& e){
    //push exception into scope
    vars.setVar(TC->exceptionName, e.what());
    interpBlock(TC->catchBlock);
  }
}

__ANY__ interpDotOnUserObjects(object * obj);

string interpDotOnString(string left, atom * right){
  string call = right->fCall->funkName;
  /*
  if(call == "at"){
    return left.at((int)interpExpression(right->fCall->parameters.at(0)).toNum());
  }else if(call == "substr"){
    return left.substr((int)interpExpression(right->fCall->parameters.at(0)).toNum(), interpExpression(right->fCall->parameters.at(1)).toNum());
  }else if(call == "insert"){
    return left.insert((int)interpExpression(right->fCall->parameters.at(0)).toNum(), interpExpression(right->fCall->parameters.at(1)).toString());
  }
  
  */
  warn("Invalid call "+call+" on type string");
  return "";
}
__ANY__ interpDot(__ANY__& left, atom * right){//this is a temp function until ther interpreter gets proper object support
  //__ANY__ left = interpAtom(leftAtom);
  string call = right->fCall->funkName;
  if(call == "at"){
    return left.at(interpExpression(right->fCall->parameters.at(0)).toNum());
  }else if(call == "toString"){
    return left.toString();
  }else if(call == "getType"){
    return left.getType();
  }else if(call == "sum"){
    return left.sum();
  }else if(call == "append"){
    left.append(interpExpression(right->fCall->parameters.at(0)));
    return left;
  }else if(call == "del"){
    left.del(interpExpression(right->fCall->parameters.at(0)).toNum());
    return left;
  }else{
    error("Unsupported call: "+call);
  }
  return __ANY__(0);

}
