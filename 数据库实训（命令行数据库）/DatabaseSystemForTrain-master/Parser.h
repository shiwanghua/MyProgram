#pragma once
#include"Lexer.h"
#include"Token.h"
#include"Stmt.h"
#include"Expr.h"
#include "DBs.h"

class Parser
{
private:
	Lexer * lex;
	Word look;
	DBs * dbs;
	string currentDatabase;

	void stmts();
	bool stmt();
	Where * matchWhere();

public:
	Parser(Lexer * l, DBs * d);
	void move();
	void error(string s);
	void match(int t);
	void program();
	void setLexer(Lexer* l);

	~Parser();
};

