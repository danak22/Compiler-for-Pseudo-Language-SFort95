/* 
 * parser(SP24).h
 * Programming Assignment 2
 * Spring 2024
*/

#ifndef PARSE_H_
#define PARSE_H_

#include <iostream>

using namespace std;

#include "lex.h"



extern bool Prog(istream& in, int& line);
extern bool Decl(istream& in, int& line);
extern bool Type(istream& in, int& line);
extern bool VarList(istream& in, int& line);
extern bool Stmt(istream& in, int& line);
extern bool SimpleStmt(istream& in, int& line);
extern bool PrintStmt(istream& in, int& line);
extern bool BlockIfStmt(istream& in, int& line);
extern bool SimpleIfStmt(istream& in, int& line);
extern bool AssignStmt(istream& in, int& line);
extern bool Var(istream& in, int& line);
extern bool ExprList(istream& in, int& line);
extern bool RelExpr(istream& in, int& line);
extern bool Expr(istream& in, int& line);
extern bool MultExpr(istream& in, int& line);
extern bool TermExpr(istream& in, int& line);
extern bool SFactor(istream& in, int& line);
extern bool Factor(istream& in, int& line, int sign);
extern int ErrCount();

#endif /* PARSE_H_ */
