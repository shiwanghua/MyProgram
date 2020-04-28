#pragma once
#include "Node.h"

class Stmt: public Node
{
public:
	static const Stmt Null;
	Stmt();
	~Stmt();
};

