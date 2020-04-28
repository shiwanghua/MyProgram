package lexer;

import java.io.IOException;
import java.io.Reader;
import java.util.Hashtable;

import symbols.Type;

public class Lexer {
	public static int line = 1;
	char peek = ' ';
	Hashtable words = new Hashtable();
	Reader reader = null;
	void reserve(Word w)	{ words.put(w.lexeme, w); }
	
	public Lexer(Reader r)	{       //   以一个Reader类为参数  ，用于读文件
		reserve(new Word("if", Tag.IF));
		reserve(new Word("else", Tag.ELSE));
		reserve(new Word("while", Tag.WHILE));
		reserve(new Word("do", Tag.DO));
		reserve(new Word("break", Tag.BREAK));
		reserve(new Word("for", Tag.FOR));
		
		reserve(Word.True); reserve(Word.False);
		reserve(Type.Double);   //  我加的
		reserve(Type.Int);    //   这里加了Int！！！
		reserve(Type.Char);
		reserve(Type.Bool); reserve(Type.Float);
		reader = r;
	}
	
	void readch() throws IOException {
		//peek = (char)System.in.read();    <--以前是这样
		peek = (char)reader.read();     // 读一个字符
	}
	boolean readch(char c) throws IOException{
		readch();
		if(peek!=c) return false;
		peek = ' ';
		return true;
	}
	
	public Token scan() throws IOException{
		for(;;readch()){
			if(peek==' '||peek=='\t'||peek=='\r') continue;
			else if(peek=='\n') line = line + 1;
			else break; 
		}
		switch(peek){
		case '&':
			if(readch('&')) return Word.and; else return new Token('&');
		case '|':
			if(readch('|')) return Word.or; else return new Token('|');
		case '=':
			if(readch('=')) return Word.eq; else return new Token('=');
		case '!':
			if(readch('=')) return Word.ne; else return new Token('!');
		case '<':
			if(readch('=')) return Word.le; else return new Token('<');
		case '>':
			if(readch('=')) return Word.ge; else return new Token('>');
		}
		
		if(Character.isDigit(peek)){
			int v = 0;
			do{
				v=10*v+Character.digit(peek, 10); readch();
			}while(Character.isDigit(peek));
			if(peek!='.') return new Num(v);
			float x = v; float d = 10;
			for(;;){
				readch();
				if(!Character.isDigit(peek)) break;
				x = x + Character.digit(peek, 10)/d; d=d*10;
			}
			return new Real(x);
		}
		
		if(Character.isLetter(peek)){
			StringBuffer b = new StringBuffer();
			do{
				b.append(peek); readch();
			}while(Character.isLetterOrDigit(peek));
			String s=b.toString();
			Word w = (Word)words.get(s);     //   如果s是int，将在words哈希表里找到一个final类型的叫做Int的Type对象
			if(w!=null) return w;
			w = new Word(s, Tag.ID);
			words.put(s, w);
			return w;     //   如果是int，则会返回一个Type类常量
		}
		
		Token tok = new Token(peek); peek=' ';
		return tok;
	}
}
