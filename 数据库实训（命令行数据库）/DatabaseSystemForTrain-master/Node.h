#pragma once
#include "Lexer.h"
#include <string>
using namespace std;

class Node {
public:
	int lexline = 0;
	Node();
	void error(string str);
};