package main;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;

import parser.Parser;
import lexer.Lexer;

public class Main {

	public static void main(String[] args) throws IOException {
		// TODO Auto-generated method stub
		// ��ȡ�ļ�
		byte b=127;
		++b;
		File file = new File("D:\\我的文件啊\\大二暑假\\数据库实训\\实验\\实验三&四\\test.txt");
		Reader reader = null;
		reader = new InputStreamReader(new FileInputStream(file));
		// �½�һ���ʷ������������ļ�����lexer��������ȡ�ļ�����
		Lexer lex = new Lexer(reader);
		Parser parser = new Parser(lex);
		parser.program();
		System.out.print("\n");
	}

}
