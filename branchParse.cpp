
void parseBranchDec(branch * B);
void parseBranchPre(branch * B);
void parseBranch(branch * B);
void addBranchCaller(branch * B);


void parseBranchDec(branch * B){
  debug("parseBranchDec()");
  B->isBranch = true;
  if(acceptType()){//change to parseProg var; take tpye/arr, name = branch name
    B->type = lastSym->tokenText;
  }else{
    B->type = "__ANY__";
  }
  expect(TokenData::IDENT);
  B->name = lastSym->tokenText;
  if(accept("extends") || accept("::")){
    expect(TokenData::IDENT);
    B->extends = lastSym->tokenText;
  }else{
    B->extends = "Object";
  }
  B->extends = "runnable";//TODO eventually itll be able to extend other things...
  expect("(");
  bool needComma = false;
  while(!accept(")")){// from void parseFunkKW(Funk* F)--really should make this its own func, is used several times
    if(!(accept(",") ^ needComma)){
      //var * a = new var();
      var a = parseVar();
      /*if(acceptType()){
        a.type = lastSym->tokenText;//TODO acceptType vs valueType?
      }else{
        a.type = "__ANY__";
        if(lintProg && currentLineOn) checkTypeAllow(a.name);//check for currentLineOn so it only prints lintProg msg durring normal parsing, not durring pre-scan
	}*/
      expect(TokenData::IDENT);//should be name of var
      a.name = lastSym->tokenText;
      if(currentLineOn){//add var to scope if not in pre-scan
        setVar(a);
        if(lintProg) checkVarName(a.name);
      }
      if(accept("="))
        a.startingValue = parseExpression(a.type);

      B->parameters.push_back(a);//TODO fix this
      B->memberVars.push_back(new var(a));//this is ugly
      needComma = true;
    }else{
      error("Missing comma in function parameter", false);//TODO make skippable
    }
  }
  debug("parseBranchDec() done");
}


void parseBranchPre(branch * B){
  parseClassPre(B);
  addBranchCaller(B);
}


void parseBranch(branch * B){
  parseClass(B);
}

void addBranchCaller(branch * B){
  debug("addBranchCaller()");
  Funk * caller = new Funk();
  caller->returnType = B->name;
  caller->name = B->name;
  caller->parameters = B->parameters;
  string code = "";
  for(unsigned i = 0;i<B->parameters.size(); i++){
    code+="this->"+B->parameters[i].name+"="+B->parameters[i].name+";\n";//set all values
  }
  code+="this->start();\n";
  parseNode * pn = new parseNode();
  pn->data = code;
  pn->type = "C++";
  caller->funkBlock->Lines.push_back(pn);
  B->constructors.push_back(caller);
  //B->memberFunks.push_back(caller);
  debug("addBranchCaller() done");
}

/*
branch int mine(string you){
var a = 0
func run(){
a++
age = this.read()
print(you+" is age "+age)
print("ive run "+a+"times)
}
}

-----------


class mine: public runnable{
Channel<int> chan;
int read(){return chan.read();}
void write(int data){chan.write(data);}//etc

threadSafe __ANY__ a;
threadSafe you;

int run(){
a++
age = this.read()
print(you+" is age "+age)
print("ive run "+a+"times)
}//run


mine(string you){
this.you = you
runnable r = runnable(this.run);
}

}//mine


}

 */



/*

int x = funkCallTahtTakesLongTime()
//do other thing that takes a while
string a = callUsingVar(x)

-------------------

future<int> x = async(funkCallTahtTakesLongTime())
//do other thing that takes a while
string a = callUsingVar(x.get())


 */
