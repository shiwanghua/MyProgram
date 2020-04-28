package lexer;

import java.io.IOException;
import java.util.Hashtable;

public class Lexer {
	public static int line = 1;
	char peek = ' ';
	Hashtable<String, Word> words = new Hashtable<String, Word>();

	void reserve(Word w) {       // 保留关键字
		words.put(w.lexeme, w);    //   往哈希表里put数据   lexeme是String类型
	}

	public Lexer() {          //   保留词素
		reserve(new Word("if", Tag.IF));
		reserve(new Word("else", Tag.ELSE));
		reserve(new Word("while", Tag.WHILE));
		reserve(new Word("do", Tag.DO));
		reserve(new Word("break", Tag.BREAK));
		reserve(Word.True);
		reserve(Word.False);
	}

	public void readch() throws IOException {    //读入一个字符并将之赋给peek
		peek = (char) System.in.read();
	}

	boolean readch(char c) throws IOException {   // 读入一个字符并判断这个字符是否与类Lexer的另一个字符peek相等
		readch();
		if (peek != c) {
			return false;
		}
		peek = ' ';
		return true;
	}

	public Token scan() throws IOException {   //  扫描函数
		for (;; readch()) {
			if (peek == ' ' || peek == '\t')    //忽略空格和制表符
				continue;
			else if (peek == '\n') {
				line += 1;
			} 
			else {
				break;     //  如果输入的不是空格，“、t”，“\n”， 则跳出for循环
			}
		}
		switch (peek) {
		case '&':
			if (readch('&'))
				return Word.and;    //   输入两个&字符，即一个关键字
			else
				return new Token('&');      //  返回一个词素
		case '|':
			if (readch('|'))
				return Word.or;
			else
				return new Token('|');
		case '=':
			if (readch('='))
				return Word.eq;
			else
				return new Token('=');
		case '!':
			if (readch('='))
				return Word.ne;
			else
				return new Token('!');
		case '<':
			if (readch('='))
				return Word.le;
			else
				return new Token('<');
		case '>':
			if (readch('='))
				return Word.ge;
			else
				return new Token('>');
		case '/':                   //  单行注释的判断
			if(readch('/'))
			{
				String s="" ;
				while(!readch('\r')){
					s+=peek;
				}
				System.out.println("(COMMENT , // "+s+")");
			return Word.Comment;
			}
			else 
				return new Token('/');
		}
		if (Character.isDigit(peek)) {    //如果输入的是一个数字
			int v = 0;
			do {
				v = 10 * v + Character.digit(peek, 10);
				readch();
			} while (Character.isDigit(peek));   //  得到一个十进制整数
			if (peek != '.')
				return new Num(v);       //  不是浮点数则返回这个整数，结束一次扫描scan  
			float x = v;
			float d = 10; 
			for (;;) {
				readch();
				if (!Character.isDigit(peek))
					break;
				x = x + Character.digit(peek, 10) / d;
				d = d * 10;
			}
			return new Real(x);      // 返回得到的浮点数
		}
		if (Character.isLetter(peek)) {
			StringBuffer b = new StringBuffer();
			do {
				b.append(peek);
				readch();
			} while (Character.isLetterOrDigit(peek));   //  把输入的字符装入StringBuffer
			String s = b.toString();
			Word w = (Word) words.get(s);
			if (w != null)    // get成功
				return w;
			w = new Word(s, Tag.ID);
			words.put(s, w);   
			return w;
		}
		Token tok = new Token(peek);
		peek = ' ';
		return tok;
	}
	
	public void out() {     // 输出哈希表的大小
		System.out.println(words.size());
		
	}

	public char getPeek() {   
		return peek;
	}

	public void setPeek(char peek) {
		this.peek = peek;
	}

}

