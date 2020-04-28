#pragma once
#include "Stmt.h"

class Seq: public Stmt {
private:
	Stmt stmt1;
	Stmt stmt2;

public:
	Seq(Stmt s1, Stmt s2);
};