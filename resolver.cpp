/*expression2 * resolveExpression2(expression3 * expIn, string targetType);
expression2 * resolveExpressionHelper(expression3 * expIn, string targetType, int start, int end, string typeCalledOn, bool isStatic, vector<unsigned long> arr);
expression2 * resolveSingle(atom * a, string targetType);
void resolveAtom(atom * a);
expression2 * resolveAtoms(atom * left, atom * right, string OP);
expression2 * resolveExpressions(expression2 * left, expression2 * right, string OP);
void applyCast(expression2 * EXP, string targetType);
void applyCast(atom * a, string targetType);*/

int recused = 0;

/*Used to tell if a var type is an object
  Object does not need ot be user created*/
bool isObj(const string& name){
    if(convertedType(name) == "num")
        return false;
    if(name == "string")
        return false;
    return true;
}

/*Checks warning/eror if a cast 'from' to 'targetType' is allowed.
The return value can be ignored, it is only used to track/throw warning and errors*/
bool checkCast(const string& from, const string& targetType, bool output = true){
    if(targetType == "string" && convertedType(from) == "num" && from != "bool"){
        if(getLintFlag("NUM_TO_STRING") == 1){
            if(output) warn("Cast from number("+from+") to string");
        }else if(getLintFlag("NUM_TO_STRING") == 2){
            if(output) error("Cast from number("+from+") to string", true);
            return false;
        }
    }else if(convertedType(targetType) == "num" && from == "string"){
        if(getLintFlag("STRING_TO_NUM") == 1){
            if(output) warn("Cast from string to number");
        }else if(getLintFlag("STRING_TO_NUM") == 2){
            if(output) error("Cast from string to number", true);
            return false;
        }
    }else if(!builtInType(from) && from != "num" && targetType != "__ANY__" && getFlag("IMPLICIT_CASTS_ON_OBJECTS"))
        warn("Implicit cast from type '"+from+"' to type "+targetType);
    //lint?
    return true;
}

string resolveTypeOfDots(const expression3 * exp){//should return var
    return exp->bigAtoms->at(0)->a->helper.type;
}

expression2 * exp3toexp2Sized(expression3 * expIn, int start, int end){
    expression3 * exp3 = new expression3();
    for(int i = start; i < end; i++){
        exp3->bigAtoms->emplace_back(expIn->bigAtoms->at(i));
    }
    expression2 * exp2 = new expression2();
    exp2->leftAtom = exp3ToAtom(exp3);
    //exp2->leftIsExp = true;
    exp2->helper = resolveTypeOfDots(exp3);
    return exp2;
}



void applyCast(expression2 * EXP, string targetType){
    //cout<<"Applying cast with target type: "<<targetType<<" and current type as: "<<EXP->helper<<endl;
    debug("applyCast(EXP: "+EXP->helper+" to "+targetType+")");
    if(convertedType(targetType) == convertedType(EXP->helper) || (targetType == "" || targetType == "*")){
        debug("applyCast(EXP) done");
        return;
    }
    checkCast(EXP->helper, targetType);
    if(targetType == "string" && convertedType(EXP->helper) == "num"){//TODO bool should be different
        //checkCast(EXP->helper,"string");
        EXP->cast = "num to string";
    }else if(targetType == "string" && EXP->helper == "__ANY__"){
        EXP->cast = "string";
    }else if(targetType == "string" && isObj(EXP->helper)){
        EXP->cast = "toString";
    }else if(convertedType(targetType) == "num" && EXP->helper == "string"){
        cout<<"123"<<endl;
        EXP->cast = "string to num";

        //TODO cast warning/check allow
        /* if(getLintFlag("STRING_TO_NUM") == 1){
          warn("Cast from string to number");
        }else if(getLintFlag("STRING_TO_NUM") == 2){
          error("Cast from string to number", true);
          }*/
    }else{
        //if(!builtInType(EXP->helper) && EXP->helper != "num" && targetType != "__ANY__" && getFlag("IMPLICIT_CASTS_ON_OBJECTS"))
        //  warn("Implicit cast from type '"+EXP->helper+"' to type "+targetType);
        EXP->cast = convertedType(targetType);
    }
    //cout<<"applyCast() set cast to: "<<EXP->cast<<endl;
    debug("applyCast(EXP) done");
}

void applyCast(atom * a, string targetType){
    debug("applyCast(atom: "+a->helper.type+" to "+targetType+")");
    if(convertedType(targetType) == convertedType(a->helper.type) || (targetType == "" || targetType == "*")){
        debug("applyCast(atom) done");
        return;
    }
    if(targetType == "string" && convertedType(a->helper.type) == "num"){//TODO bool should be different
        a->cast = "num to string";
        checkCast(a->helper.type, targetType);
    }else if(convertedType(targetType) == "num" && a->helper.type == "string"){
        a->cast = "string to num";
    }else{
        if(!builtInType(a->helper.type) && getFlag("IMPLICIT_CASTS_ON_OBJECTS"))
            warn("Implicit cast from type '"+a->helper.type+"' to type "+targetType);
        a->cast = convertedType(targetType);
    }
    debug("applyCast(atom: "+targetType+") done");
}

void printAtom(atom * a){
    out = "";
    int tmp = reportingLevel;
    reportingLevel = 2;
    TransAtom(a);
    reportingLevel = tmp;
    cout<<out<<endl;
    out="";
}

void printExp(expression2 * EXP){
    out = "";
    int tmp = reportingLevel;
    reportingLevel = 2;
    TransExp(EXP);
    reportingLevel = tmp;
    cout<<out<<endl;
    out="";
}
void printExp(expression3 * EXP){
    int tmp = reportingLevel;
    out = "";
    TransExp(EXP);
    cout<<out<<endl;
    out="";
    reportingLevel = tmp;
}
void printExp(expression3 * EXP, int start, int end){
    int tmp = reportingLevel;
    if(end==-1)
        end = EXP->bigAtoms->size();
    cout<<"start = "<<start<<" end = "<<end<<endl;
    for(int i = start; i < end; i++){
        if(EXP->bigAtoms->at(i)->type == bigAtom::ATOM){
            out = "";
            reportingLevel = 2;
            TransAtom(EXP->bigAtoms->at(i)->a);
            reportingLevel = -2;
            cout<<out;
            out = "";
        }else{
            cout<<EXP->bigAtoms->at(i)->op;
        }
    }
    cout<<endl;
    reportingLevel = tmp;
}

expression2 * resolveExpression2(expression3 * expIn, var targetType){//helper function to supply all needed starting values

    return resolveExpressionHelper(expIn, targetType.type, 0, expIn->bigAtoms->size(), "void", false);
}

expression2 * resolveExpression2(expression3 * expIn, string targetType){//helper function to supply all needed starting values
    return resolveExpressionHelper(expIn, targetType, 0, expIn->bigAtoms->size(), "void", false);
}

//expression2 * resolveExpressionHelper(expression3 * expIn, string targetType, int start, int end, string typeCalledOn = "", bool isStatic = false, vector<unsigned long> arr = {}){
expression2 * resolveExpressionHelper(expression3 * expIn, string targetType, const int start, const int end, string typeCalledOn,  bool isStatic, vector<unsigned long> arr){
    expression2 * EXP = new expression2();
    debug("resolveExpressionHelper("+targetType+")");
    recused++;
    //report(to_string((long long)start)+" / "+to_string((long long)end), -2);
    //printExp(expIn, start, end);
    //check for errors
    if(start-end >= 0)
        throw invalid_argument("Cant resolve, start/end: "+to_string((long long)start)+" / "+to_string((long long)end));

    //printExp(expIn, start, end);

    //parse trivial cases
    if(end - start == 1){
        //cout<<"Get trivial: size of 1"<<endl;
        if(expIn->bigAtoms->at(start)->type == bigAtom::ATOM){
            debug("resolveExpressionHelper() done");
            return resolveSingle(expIn->bigAtoms->at(start)->a, targetType, arr);
        }else{
            throw invalid_argument("start/end: "+to_string((long long)start)+" / "+to_string((long long)end)+" resolve single is an OP");
        }

    }else if(end - start == 2){
        //find the OP, then set
        if(expIn->bigAtoms->at(start)->type == bigAtom::OP){
            EXP->OP = expIn->bigAtoms->at(start)->op;
            if(expIn->bigAtoms->at(start)->op != ".")
                resolveAtom(expIn->bigAtoms->at(start+1)->a);
            EXP->rightAtom = (expIn->bigAtoms->at(start+1)->a);
            EXP->rightIsExp = false;//should already be set
            EXP->helper = EXP->rightAtom->helper.type;
            //cout<<EXP->helper<<" "<<getOP(EXP->OP).returnsBool<<endl;
            if(EXP->helper == "__ANY__" && getOP(EXP->OP).returnsBool){
                EXP->helper = "bool";
            }
        }else{
            EXP->OP = expIn->bigAtoms->at(start+1)->op;
            if(expIn->bigAtoms->at(start)->op != ".")
                resolveAtom(expIn->bigAtoms->at(start)->a);//TODO this is a bad way of doing things
            EXP->leftAtom = (expIn->bigAtoms->at(start)->a);
            EXP->leftIsExp = false;//should already be set
            EXP->helper = EXP->leftAtom->helper.type;
        }
    }else if(end-start == 3){
        //cout<<"Get trivial: size of 3"<<endl;
        EXP = resolveAtoms(expIn->bigAtoms->at(start)->a, (expIn->bigAtoms->at(start+2)->a), expIn->bigAtoms->at(start+1)->op);
    }else{

        //get *LOWEST* priorety OP position
        int position = -1, highestPriority = 999, dots = 0;
        for(int i = start; i < end/*expIn->bigAtoms->size()*/; i++){
            if(expIn->bigAtoms->at(i)->type == bigAtom::OP && getOP(expIn->bigAtoms->at(i)->op).priority < highestPriority){//>= to go right to left
                if(expIn->bigAtoms->at(i)->op == "."){
                    dots++;
                    position = i;
                    highestPriority = getOP(expIn->bigAtoms->at(i)->op).priority;
                }else{
                    position = i;
                    highestPriority = getOP(expIn->bigAtoms->at(i)->op).priority;
                }
            }
        }
        //cout<<"Next OP is: "<<expIn->bigAtoms->at(position)->op<<" at position "<<position<<endl;
        if(position == -1)
            throw invalid_argument("I have failed you: postion at -1 in resolveExpressionHelper("+EXP->helper+")");

        report("Next OP is: "+expIn->bigAtoms->at(position)->op+" at position "+::toString(position), -4);
        if(position != -1){
            EXP = resolveExpressions(resolveExpressionHelper(expIn, "", start, position, typeCalledOn, isStatic), resolveExpressionHelper(expIn, "", position+1, end, typeCalledOn, isStatic), expIn->bigAtoms->at(position)->op);
            applyCast(EXP, targetType);
        }else{
            EXP = exp3toexp2Sized(expIn, start, end);
            //EXP->helper = resolveTypeOfDots(expIn);
            //TODO apply some cast
        }
    }
    if(EXP->helper != targetType && targetType != "" || targetType != "*")
        applyCast(EXP, targetType);
    //delete expIn;
    debug("resolveExpressionHelper("+EXP->helper+") done");
    return EXP;
}

//Turns exp3 an atom has into an exp2
void resolveAtom(atom * a){
    debug("resolveAtom()--");
    if(a->type == "exp3"){
        //cout<<"Got exp3 here"<<endl;
        expression3 * tmp = a->exp3;
        a->type = "exp2";
        a->exp2 = resolveExpression2(a->exp3, "*");
        //a = a->exp3->bigAtoms->at(0)->a;
        tmp->~expression3();
        delete tmp;
        a->helper = a->exp2->helper;
    }
}

expression2 * resolveSingle(atom * a, string targetType, vector<unsigned long> arr){
    debug("resolveSingle("+targetType+")--");
    expression2 * EXP = new expression2();
    resolveAtom(a);
    //applyCast(a, targetType);
    EXP->leftAtom = a;
    EXP->leftIsExp = false;
    EXP->helper = a->helper.type;
    applyCast(EXP, targetType);
    return EXP;
}


expression2 * resolveExpressions(expression2 * left, expression2 * right, string OP){
    debug("resolveExpressions!!()");
    expression2 * EXP = new expression2();
    EXP->OP = OP;
    //cout<<"Left helper: "<<left->helper<<" left converterd: "<<convertedType(left->helper)<<" left cast: "<<left->cast<<" ";printExp(left);
    //cout<<"Right helper: "<<right->helper<<" right converterd: "<<convertedType(right->helper)<<" right cast: "<<right->cast<<" ";printExp(right);
    //cout<<"OP: "<<OP<<endl;
    if(OP != "."){
        if(getOP(OP).returnsBool){
            //cout<<"Cast: "<<"bool OP"<<endl;
            applyCast(right, left->helper);//if its a bool OP(==, <=, etc..) make the right side the same type as left side
            EXP->helper = "bool";
        }else if(OP == "*" && convertedType(left->helper) == "string" && convertedType(right->helper) == "num"){
            //cout<<"string * num"<<endl;
            left->cast = "__ANY__";
            EXP->helper = "__ANY__";
        }else if(OP == "*" && convertedType(left->helper) == "num" && convertedType(right->helper) == "string"){
            //cout<<"num * string"<<endl;
            left->cast = "__ANY__";
            right->cast = "__ANY__";
            EXP->helper = "__ANY__";
        }
        else if(convertedType(left->helper) == convertedType(right->helper)){//if they are the same, were good
            //cout<<"same type"<<endl;
            EXP->helper = convertedType(right->helper);
        }else{
            //------ <ONE> ---------//matches assignment baised on assocativity
            /*
            bool matchLeft = getOP(OP).leftToRightAssociativity;
            if(matchLeft){
          //right->cast = convertedType(left->helper);
          if(convertedType(left->helper) != "__ANY__")
            applyCast(right, convertedType(left->helper));
          EXP->helper = left->helper;
            }else{
          //left->cast = convertedType(right->helper);
          if(convertedType(right->helper) != "__ANY__")
            applyCast(left, convertedType(right->helper));
          EXP->helper = right->helper;
            }
          }
            */
            //---------- </ONE> ---------
            //--------- <TWO> ----------//matches types baised on type
            if(left->helper == "__ANY__" || right->helper == "__ANY__"){//if one is an any, they both have to be
                if(left->helper != "__ANY__")
                    left->cast = "__ANY__";
                if(right->helper != "__ANY__")
                    right->cast = "__ANY__";
                EXP->helper = "__ANY__";
            }else if(left->helper == "string"){//if one is a string, they both have to be
                if(convertedType(right->helper) == "num"){
                    right->cast = "num to string";
                }else{
                    right->cast = "string";
                }
                EXP->helper = "string";
            }else if(right->helper == "string"){
                if(convertedType(left->helper) == "num"){
                    left->cast = "num to string";
                }else{
                    left->cast = "string";
                }
                EXP->helper = "string";
            }else{
                throw invalid_argument("ERROR in resolveExpressions");
            }
        }
        //---------- </TWO> --------------
    }else{
        EXP->helper = right->helper;
    }
    EXP->leftExp = left;
    EXP->rightExp = right;
    EXP->rightIsExp = true;
    EXP->leftIsExp = true;
    //cout<<"Post Left helper: "<<left->helper<<" left converterd: "<<convertedType(left->helper)<<" left cast: "<<left->cast<<" ";printExp(left);
    //cout<<"Post Right helper: "<<right->helper<<" right converterd: "<<convertedType(right->helper)<<" right cast: "<<right->cast<<" ";printExp(right);
    debug("resolveExpressions!!("+EXP->helper+") done");
    return EXP;
}
expression2 * resolveAtoms(atom * left, atom * right, string OP){
    debug("resolveAtoms()");
    expression2 * EXP = new expression2();
    EXP->OP = OP;
    //resolveAtom(left);
    //resolveAtom(right);
    //cout<<"Left type: "<<left->type<<" left helper: "<<left->helper<<" left converterd: "<<convertedType(left->helper)<<" ";printAtom(left);
    //cout<<"Right type: "<<right->type<<" right helper: "<<right->helper<<" right converterd: "<<convertedType(right->helper)<<" ";printAtom(right);
    if(OP != "."){ //&& OP != "[" && OP != "]" ){
        if(getOP(OP).returnsBool){
            //cout<<"Cast: "<<"bool op"<<endl;
            applyCast(right, left->helper.type);//if its a bool OP(==, <=, etc..) make the right side the same type as left side
            EXP->helper = "bool";
        }else if(OP == "*" && convertedType(left->helper.type) == "string" && convertedType(right->helper.type) == "num"){
            //cout<<"Cast: "<<"str*num"<<endl;
            left->cast = "__ANY__";
            //left->stringLit = false;
            EXP->helper = "__ANY__";
        }else if(OP == "-" && convertedType(left->helper.type) == "string" && convertedType(right->helper.type) == "num"){
            //cout<<"Cast: "<<"str-num"<<endl;
            left->cast = "__ANY__";
            //left->stringLit = false;
            EXP->helper = "__ANY__";
        }else if(OP == "*" && convertedType(left->helper.type) == "num" && convertedType(right->helper.type) == "string"){
            //cout<<"Cast: "<<"num*str"<<endl;
            left->cast = "__ANY__";
            right->cast = "__ANY__";
            EXP->helper = "__ANY__";
        }else if(convertedType(left->helper.type) == convertedType(right->helper.type)){//if they are the same, were good
            //cout<<"Cast: "<<"same type"<<endl;
            EXP->helper = convertedType(right->helper.type);
        }else{
            if(left->helper.type == "__ANY__" || right->helper.type == "__ANY__"){//if one is an any, they both have to be
                //cout<<"Cast: "<<"any"<<endl;
                if(left->helper.type != "__ANY__")
                    left->cast = "__ANY__";
                if(right->helper.type != "__ANY__")
                    right->cast = "__ANY__";
                EXP->helper = "__ANY__";
            }else if(left->helper.type == "string"){//if one is a string, they both have to be
                //cout<<"Cast: "<<"left str"<<endl;
                checkCast(right->helper.type, "string");
                if(convertedType(right->helper.type) == "num"){
                    right->cast = "num to string";
                }else{
                    right->cast = "string";
                }
                EXP->helper = "string";
            }else if(right->helper.type == "string"){
                //cout<<"Cast: "<<"right str"<<endl;
                if(convertedType(left->helper.type) == "num"){
                    //cout<<"num to string on left"<<endl;
                    checkCast(left->helper.type, "string");
                    left->cast = "num to string";
                }else{
                    left->cast = "string";
                }
                EXP->helper = "string";
            }else if(right->helper.type == "void"){
                left->cast = "";
                EXP->helper = "void";
            }else{
                cout<<"ERROR!"<<endl;
                printAtom(left);
                printAtom(right);
                throw invalid_argument("ERROR in resolveAtoms; right->helper = "+right->helper.toString()+" :: left->helper = "+ left->helper.toString());
            }
        }
    }else{
        EXP->helper = right->helper.type;
    }
    EXP->leftAtom = left;
    EXP->rightAtom = right;
    EXP->rightIsExp = false;
    EXP->leftIsExp = false;

    debug("resolveAtoms("+EXP->helper+") done");
    return EXP;
}


/*
bool parseAtom(atom * a, string base, bool baseStatic){
  debug("parseAtom()");
  bool baseIsStatic = true;
  if(accept(TokenData::IDENT)){
    string identName = lastSym->tokenText;
    if(peek("(")){//must be a function call
      Funk *tmp = getFunk(identName, base);
      if(tmp == NULL){
	if(base){
	  error("No function "+identName+" in class "+base, false);
	}else
	  error("No function "+identName+" found", false);
      }
      if(base != "" && baseStatic && !tmp->STATIC)
	error("Non static call ("+tmp->name+") being called on non-static object ("+lastSym->tokenText, true);
      a->type = "funkCall";
      funkCall * FC = new funkCall();
      a->fCall = FC;
      a->helper = tmp->returnType;
      parseFunkCall(a->fCall,identName, base);//TODO make it so we can pass func(*tmp) so it doenst have to get again?
    }else{//must be var
      a->type = "lit";
      if(base){//could be member var
	var * varT = getMemberVar(base, identName);
	if(varT != NULL){
	  a->literalValue = &(varT->name);
	  a->helper = varT->type;
	  //a->baseIsStatic = varT->STATIC;
	  //baseIsStatic = varT->STATIC;
	  debug("parseAtom() done");
	  return varT->STATIC;
	}
      }
      //check in scope vars now
      string tp = getVARType(lastSym->tokenText);
      if(tp == "DNE"){
	error("Undeclaired var or function('"+lastSym->tokenText+"') +in base: "+base, true);
	a->helper = "__ANY__";//what should it defualt to?
      }else{
	a->helper = tp;
      }
      a->literalValue = &(lastSym->tokenText);
      //TODO get if its static
    }
  }else if(acceptType()){
    a->helper = lastSym->tokenText;
    //a->baseIsStatic = true;
    a->type = "lit";
    a->literalValue = &(lastSym->tokenText);
  }else if(accept("this")){
    if(currentFunk->STATIC)
      error("Cant use 'this' in static function");
    a->helper = lastSym->tokenText;
    //a->baseIsStatic = false;
    baseIsStatic = false;
    a->type = "lit";
    a->literalValue = new string("(*this)");
  }else if(accept("(")){
    a->type = "exp3";
    a->exp3 = parseExpression();
    a->cast = "";
    a->helper = "exp3";
    expect(")");
  }else if(peek("-") && (sym+1)->TokenType == TokenData::NUM){
    a->type = "lit";
    a->helper = "num";
    a->literalValue = new string(twoSymbAgo->tokenText + lastSym->tokenText);
  }else{
    //shoudl be hard value: ie 5 or "hello" or false
    if(accept(TokenData::NUM) || accept("true") || accept("false")){
      a->type = "lit";
      a->helper = "num";//TODO may have to do bool for bool
      a->literalValue = &lastSym->tokenText;
    }else if(accept(TokenData::STRING_LITERAL)){
      a->type = "lit";
      a->helper = "string";
      a->literalValue = &lastSym->tokenText;
    }else{
      error("Unkown type", true);
      a->type = "__ANY__";
      a->helper = "__ANY__";
    }
  }  
  debug("parseAtom() done");
  return baseIsStatic;
}


/*
void parseAtom(atom * a,string base, bool baseStatic){
  debug("parseAtom()");
  if(peek("(")){//func call
    Funk *tmp = getFunk(identName, base);
    if(tmp == NULL)
      error("No function "+identName+(base==""?"":" in class "+base, false));
    if(base != "" && baseStatic && !tmp->STATIC)//TODO fix constructos being static
      error("Non static call ("+tmp->name+") being called on non-static object ("+lastSym->tokenText, true);
    a->type = "funkCall";
    funkCall * FC = new funkCall();
    a->fCall = FC;
    a->helper = tmp->returnType;
    parseFunkCall(a->fCall,identName, base);
  }else{
    
    
  }else if(accept(")")){
    a->type = "exp3";
    a->exp3 = parseExpression();
    a->helper = "*";
  }else{
    
  }
  debug("parseAtom() done");
}





//old code for parsing dot
 /*
        if accept dot,
        make new exp
        do while(accept dot){mini exp parse of dots}
        set type of EXP to expected type
       */

/*      if(accept(".")){
        expression3 * expDot = new expression3;
        atom * aDot = new atom();
        aDot->type = "exp3";
        aDot->exp3 = expDot;
        expDot->bigAtoms->push_back(&(*EXP->bigAtoms->at(EXP->bigAtoms->size()-1)));
        //EXP->bigAtoms->pop_back();
        EXP->bigAtoms->erase(EXP->bigAtoms->end());
        bigAtom * bDot = new bigAtom();
        bDot->type = bigAtom::ATOM;
        bDot->a=aDot;
        EXP->bigAtoms->push_back(bDot);
        do{
          bigAtom * nAtom = new bigAtom();nAtom->type = bigAtom::OP; nAtom->op = lastSym->tokenText; expDot->bigAtoms->push_back(nAtom);
          //parse dot
          //string type = getType(twoSymbAgo->tokenText);
          if(accept("(")){
            if(getFunk(lastSym->tokenText, beingCalledOn) == NULL){
              error("Missing call to function "+twoSymbAgo->tokenText+" in class "+beingCalledOn, false);
            }else{
              //parseFunk
              atom * tmp = new atom;
              tmp->type = "funk";

              tmp->fCall = new funkCall;
              parseFunkCall(tmp->fCall, twoSymbAgo->tokenText);//TODO
            }
          }else{
            //parse class var
            if(getMemberVar(expDot->bigAtoms->at(expDot->bigAtoms->size() -1)->a->helper, lastSym->tokenText) != NULL){
              //youre good?
              atom * tmp = new atom;
              tmp->type = "lit";
              tmp->literalValue = &lastSym->tokenText;
              tmp->helper = getMemberVar(expDot->bigAtoms->at(expDot->bigAtoms->size() -1)->a->helper, lastSym->tokenText)->type;
	      }else{
              error("No var "+lastSym->tokenText+" found in class "+/*TODO*///" class?", false);
/*            }
          }
        }while(accept("."));
      }//end parse dot



//old parseAtom on ident

if(accept(TokenData::IDENT)){
       Funk *tmp = getFunk(lastSym->tokenText, "*");
       cout<<"Tmp is: "<<tmp<<endl;
       if(tmp != NULL){
       //if(isCallable(lastSym->tokenText)){
         //parse function call
         a->type = "funkCall";
         funkCall * FC = new funkCall();
         a->fCall = FC;
         a->helper = tmp->returnType;
         parseFunkCall(a->fCall,lastSym->tokenText, base);
       }else{
         a->type = "lit";//not true but its easy
         string tp = getVARType(lastSym->tokenText);
         report("type was: "+tp, -2);
         if(tp == "DNE"){
           error("Undeclaired var or function('"+lastSym->tokenText+"')", true);
           a->helper = "__ANY__";//what should it defualt to?
         }else{
           a->helper = tp;
         }
         a->literalValue = &(lastSym->tokenText);
       }

 }

//old parseAtom

void parseAtom(atom * a,string targetType, string base, bool baseStatic){//TODO . OP here TODO reorder for efficency
     debug("parseAtom()");
     a->cast = "";
     if(base == "")
       base = "*";
     if(accept(TokenData::IDENT) || accept("this")){
       string identName = lastSym->tokenText;
       if(peek("(")){
         Funk *tmp = getFunk(identName, base);
         if(tmp == NULL)
           error("No function "+identName+" in class "+base, false);
         if(base != "" && baseStatic && !tmp->STATIC)//TODO fix constructos being static
           error("Non static call ("+tmp->name+") being called on non-static object ("+lastSym->tokenText, true);
         a->type = "funkCall";
         funkCall * FC = new funkCall();
         a->fCall = FC;
         a->helper = tmp->returnType;
         parseFunkCall(a->fCall,identName, base);
       }else if(base == "" || base == "*"){
         a->type = "lit";//not true but its easy
         string tp = getVARType(lastSym->tokenText);
         report("type was: "+tp, -2);
         if(tp == "DNE"){
           error("Undeclaired var or function('"+lastSym->tokenText+"') +in base: "+base, true);
           a->helper = "__ANY__";//what should it defualt to?
         }else{
           a->helper = tp;
         }
         a->literalValue = &(lastSym->tokenText);
         if(*(a->literalValue) == "this")//TODO not the best way to do this
           a->literalValue = new string("(*this)");
       }else{
         //check if member var
         var * varT = getMemberVar(base, lastSym->tokenText);
         if(varT == NULL)
           error("Undefined var "+lastSym->tokenText+" in class "+base);
         a->literalValue = &(varT->name);
         a->type = "lit";
         a->helper = varT->type;
         a->baseIsStatic = false;
       }
     }else if(accept("(")){
       a->type = "exp3";
       a->exp3 = parseExpression(targetType);
       a->cast = "";
       a->helper = targetType;//TODO
       expect(")");
     }else if(accept("-") && (accept(TokenData::NUM) || accept(TokenData::IDENT))) {
       a->type = "lit";
       a->helper = "num";
       a->literalValue = new string(twoSymbAgo->tokenText + lastSym->tokenText);
     }else{
       //shoudl be hard value: ie 5 or "hello" or false
       if(accept(TokenData::NUM) || accept("true") || accept("false")){
         a->type = "lit";
         a->helper = "num";
         a->literalValue = &lastSym->tokenText;
       }else if(accept(TokenData::STRING_LITERAL)){
          a->type = "lit";
          a->helper = "string";
          a->literalValue = &lastSym->tokenText;
       }else{
         error("Unkown type", true);
         a->type = "__ANY__";
         a->helper = "__ANY__";
       }
     }
     debug("parseAtom() done");
}
*/
