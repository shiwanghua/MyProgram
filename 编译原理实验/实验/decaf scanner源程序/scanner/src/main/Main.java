package main;

import java.io.IOException;
import java.util.Hashtable;

import lexer.Lexer;
import lexer.Token;

public class Main {
	public static void main(String[] args) throws IOException {
		Lexer lexer = new Lexer();
		char c;
		do {         
			Token token=lexer.scan();    //调用扫描函数，返回由输入得到的一个Token类
			switch (token.tag) {         //根据这个Token的tag判断进入哪个case，输出相应的键值对 (Kind, Value)
			case 100:
				break;
			case 270:
				System.out.println("(NUM , "+token.toString()+")");
				break;
			case 272:
				System.out.println("(NUM , "+token.toString()+")");
				break;
			case 264:
				System.out.println("(ID , "+token.toString()+")");
				break;
			case 256:
				System.out.println("(ID ,"+token.toString()+")");
				break;
			case 257:
				System.out.println("(ID ,"+token.toString()+")");
				break;
			case 258:
				System.out.println("(KEY ,"+token.toString()+")");
				break;
			case 259:
				System.out.println("(KEY ,"+token.toString()+")");
				break;
			case 260:
				System.out.println("(KEY ,"+token.toString()+")");
				break;
			case 265:
				System.out.println("(KEY ,"+token.toString()+")");
				break;
			case 274:
				System.out.println("(KEY ,"+token.toString()+")");
				break;
			case 275:
				System.out.println("(KEY , "+token.toString()+")");
				break;
			case 13:               //  用于判断回车的
				break;
			case 95:
				System.out.println("(KEY , "+token.toString()+")");
				break;
			default:
				System.out.println("(SYM: A mistake messsge ! "+token.toString()+")");
				break;			}
			
		} while (lexer.getPeek()!='\n');   //  这个判断没啥用的
	}
}

