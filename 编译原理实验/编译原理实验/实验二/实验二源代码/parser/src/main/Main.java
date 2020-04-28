package main;

import java.io.IOException;

import parser.Parser;
import lexer.Lexer;

public class Main {

	public static void main(String[] args) throws IOException {
		// TODO Auto-generated method stub
		Lexer lex = new Lexer();//创建一个词法分析器
		Parser parser = new Parser(lex); //创建一个语法分析器
		parser.program();
		System.out.print("\n");
	}

}
