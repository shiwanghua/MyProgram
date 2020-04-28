#pragma once
#include "Node.h"
#include "Token.h"

class Expr: public Node
{
public:
	Expr( Token tok);
	~Expr();

private:
	Token op;
};