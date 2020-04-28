#include "Parser.h"
#include "Stmt.h"
#include "Seq.h"
#include "Tag.h"
#include "Table.h"
#include "Word.h"
#include <string>
#include <cstdlib>
#include <iostream>
using namespace std;



Parser::Parser(Lexer * lexer, DBs * d)
{
	lex = lexer;
	dbs = d;
	move();
}

void Parser::move() {
	look = lex->scan();
}

void Parser::match(int t) {
	if (look.getTag() == t) move();
	else error("syntax error");
}

void Parser::error(string str) {
	cout << "near line " << lex->getLine() << ": " << str;
	exit(1);
}

void Parser::program() {
	stmts();
}

void Parser::setLexer(Lexer* l)
{
	lex = l;
	move();
}

void Parser::stmts() {
	//return * new Seq(stmt(), stmts());
	while (stmt()) ;
}

bool Parser::stmt() {
	Expr * x;
	Stmt * s;
	Stmt * savedStmt;

	switch (look.getTag())
	{
	case ';':
		move();
	case Tag::CREATE: {
		move();
		switch (look.getTag()) {
		case Tag::TABLE: {
			move();
			if (look.getTag() == Tag::IDENTIFIER) {
				Table * table = new Table(look.lexeme);
				move();
				match('(');
				while (look.getTag() != ')') {
					if (look.getTag() != Tag::IDENTIFIER) error("syntax error");
					string colname = look.lexeme;
					move();
					match(Tag::VARCHAR);
					match('(');
					if (look.getTag() != Tag::NUMBER) error("syntax error");
					int length = stoi(look.lexeme);
					move();
					match(')');

					bool isPrimary = false, isNotNull = false; 
					while (look.getTag() != ',' && look.getTag() != ')') {
						switch (look.getTag()) {
						case Tag::PRIMARY:
							move(); match(Tag::KEY);
							isPrimary = true;
							break;
						case Tag::NOT:
							move(); match(Tag::Null);
							isNotNull = true;
							break;
						case Tag::Null:
							move();
							break;
						}
					}
					if (look.getTag() == ',') move();

					table->addColumn(new Column(colname, varchar, length, isPrimary, !isNotNull));
				}
				match(')'); match(';');
				dbs->createTable(currentDatabase, table);
			}
			else error("syntax error");
			break;
		}
			
		case Tag::DATABASE: {
			move();
			if (look.getTag() != Tag::IDENTIFIER) error("syntax error");
			string dataname = look.lexeme;
			dbs->createDatabase(new Database(dataname));
			move();
			match(';');
		}
			
		}
		break;
	}
		
	case Tag::DROP: {
		move();
		switch (look.getTag()) {
		case Tag::TABLE:
			move();
			if (look.getTag() == Tag::IDENTIFIER) {
				dbs->dropTable(currentDatabase, look.lexeme);
				move();
			}
			else error("syntax error");
			break;
		case Tag::DATABASE:
			move();
			if (look.getTag() != Tag::IDENTIFIER) error("syntax error");
			string dataname = look.lexeme;
			dbs->dropDatabase(dataname);
			move();
			break;
		}
		match(';');
		break;
	}
	case Tag::INSERT: {
		move();
		match(Tag::INTO);
		if (look.getTag() != Tag::IDENTIFIER) error("syntax error");
		string table_name = look.lexeme;
		move();

		bool hasKey = false;
		if (look.getTag() == '(') {
			hasKey = true; int length = 0;
			vector<string> strs;

			move();
			while (look.getTag() != ')') {
				if (look.getTag() != Tag::IDENTIFIER) error("syntax error");
				strs.push_back(look.lexeme);
				move();
				if (look.getTag() == ',') move();
				length++;
			}
			match(')');
			match(Tag::VALUES);
			match('(');
			map<string, string> keyValue;
			for (int i = 0; i < length; ++i) {
				if (i != 0) match(',');
				if (look.getTag() != Tag::DQMCONTENT && look.getTag() != Tag::NUMBER) error("syntax error");
				keyValue.insert(pair<string, string>(strs[i], look.lexeme));
				move();
			}
			match(')');
			dbs->insert(currentDatabase, table_name, keyValue);

		}
		else {
			int colcount = dbs->getTableColumnCount(currentDatabase, table_name);
			string * strs = new string[colcount];
			match(Tag::VALUES);
			match('(');
			for (int i = 0; i < colcount; ++i) {
				if (i != 0) match(',');
				if (look.getTag() != Tag::DQMCONTENT && look.getTag() != Tag::NUMBER) error("syntax error");
				strs[i] = look.lexeme;
				move();
			}
			match(')');
			dbs->insert(currentDatabase, table_name, strs);
		}		
		match(';');
		break;
	}
	case Tag::UPDATE: {
		move();
		if (look.getTag() != Tag::IDENTIFIER) error("syntax error");
		string tablename = look.lexeme;
		move();
		match(Tag::SET);
		map<string, string> keyValue;
		while (look.getTag() == Tag::IDENTIFIER) {
			string colname = look.lexeme;
			move(); match(Tag::EQUAL);
			string value = look.lexeme;
			move();
			if (look.getTag() == Tag::CMA) move();
			keyValue[colname] = value;
		}

		if (look.getTag() == Tag::WHERE) {
			Where * we = matchWhere();
			dbs->update(currentDatabase, tablename, keyValue, *we);
		}
		else dbs->update(currentDatabase, tablename, keyValue);
		match(';');
		break;
	}
	case Tag::DELETE: {
		move();
		match(Tag::FROM);
		if (look.getTag() != Tag::IDENTIFIER) error("syntax error");
		string tablename = look.lexeme;
		move();
		if (look.getTag() == Tag::WHERE) {
			Where * we = matchWhere();
			dbs->Delete(currentDatabase, tablename, *we);
		}
		else dbs->Delete(currentDatabase, tablename);
		match(';');
		break;
	}
	case Tag::SELECT: {
		move();
		vector<string> strs;
		while (look.getTag() == Tag::IDENTIFIER) {
			strs.push_back(look.lexeme);
			move(); 
			if (look.getTag() == Tag::CMA) move();
		}

		match(Tag::FROM);
		if (look.getTag() != Tag::IDENTIFIER) error("syntax error");
		string tablename = look.lexeme;
		move();
		if (look.getTag() == Tag::WHERE) {
			Where * we = matchWhere();
			dbs->select(currentDatabase, tablename, strs, *we);
		}
		else {
			dbs->select(currentDatabase, tablename, strs);
		}
		match(';');
		break;
	}
	case Tag::USE: {
		move();
		if (look.getTag() != Tag::IDENTIFIER) error("syntax error");
		currentDatabase = look.lexeme;
		move();
		match(';');
		break;
	}
	case Tag::EXIT: {
		cout << "exit" << endl;
		return false;
	}
	}
	return true;
}

Where * Parser::matchWhere() {
	move();
	if (look.getTag() != Tag::IDENTIFIER) error("syntax error");
	string colname = look.lexeme;
	move();
	match(Tag::EQUAL);
	string value = look.lexeme;
	move();
	return new Where(colname, value);
}

Parser::~Parser()
{
}
