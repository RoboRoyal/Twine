/*
 * Trans.h
 *
 *  Created on: Jul 28, 2018
 *      Author: Dakota
 */

#ifndef TRANS_H_
#define TRANS_H_


#include "utils/Twine_Utils.h"//read/write
#include "Operators.h"
#include "parserNodes.h"
#include "Flags.h"

using namespace std;
int indentT = 0;
int currentIttNum = 0;

//string out;//output text of file--moved to utils
//string outFileName;


void TransForwardDeclair();
bool TransProg();
void TransFunkKW(Funk * N, bool regFunk = true, bool includeStartingValue = false);
bool TransFunk(Funk * N, bool regFunk = true);
bool TransBlock(Block * B);
bool TransLine(parseNode * N);

bool TransVarAss(varAssign * N);

bool TransExp(expression2 * N);
void TransExp(expression3 * N);
void TransExpHelp(expression3 * N);
bool TransAtom(atom * N);
bool TransFunkCall(funkCall *N);

void TransVar(var * N,bool incudeStartingValue = true);
void TransClassVar(var * N, string obj);
bool Trans(string outFileName, vector<Funk> * Prog);
//bool Trans(string outFileName);

bool TransThrow(throw_e * e);
bool TransTRY(tryCatch * N);
bool TransIF(IF * N);
bool TransWHILE(WHILE * N);
bool TransFOR(FOR * N);
bool TransFOR_IN(FOR_IN * N);
void TransFOR2(FOR2 * N);

bool TransSingleComment(parseNode * N);
bool TransBlockComment(parseNode * N);

bool tempStart();
void tempEnd();
void TransClass(object * N);
void TransToString(object * N);

const string varListDef(var * v);
const string resolveVarType(var * v);

void write();
void debugT(const string& s);

#endif //TRANS_H_
