package lexer;

import java.io.IOException;
import java.util.Hashtable;

public class Lexer {

	public static int line = 1;
	char peek = ' ';   //一个空字符，用来储存输入的文本
	Hashtable words = new Hashtable();  //用默认的初始容量 (11) 和加载因子 (0.75) 构造一个新的空哈希表。
	void reserve(Word w) {  // 哈希表保留函数  w.lexeme 为键  w 为值
		words.put(w.lexeme, w);
	}

	public Lexer() {  //哈希表初始化
		reserve(new Word("if", Tag.IF));  // 键 if
		reserve(new Word("else", Tag.ELSE));  // 键 else
		reserve(new Word("while", Tag.WHILE));  //键 while
		reserve(new Word("do", Tag.DO));  //键 do
		reserve(new Word("break", Tag.BREAK));  //键 break
		reserve(Word.True); //键 true
		reserve(Word.False);  //键 false
	}
	 //读入用户输入字符，储存在字符变量 peek 中
	public void readch() throws IOException { 
		peek = (char) System.in.read();
		
	}
	//实现 最长子串匹配原则 而需要的 readch 函数
	boolean readch(char c) throws IOException {
		readch();
		if (peek != c) {  //若输入不是连续的c 则返回 false
			return false;
		}
		peek = ' ';  //若是连续的c 则返回true 并将peek 置为空
		return true;
	}

	public Token scan() throws IOException {  //扫描用户输入的字符串
		for (;; readch()) {  //扫描输入中的空格 制表符 换行符 。忽略空格，制表符。换行符使行数增加。
			if (peek == ' ' || peek == '\t')  //遇到空格或制表符 重新开始for循环
				continue;
			else if (peek == '\n') {  //遇到换行符，行数加一
				line += 1;
			} else {
				break;
			}
		}
		//空的内容排除后，提取真正的字符，并与语言保留字匹配
		switch (peek) {  //字符型peek被默认转换为整形
		case '&':   //若有连续两个&&符号，则与保留字and匹配
			if (readch('&')) 
				return Word.and;
			else  //只有一个&符号 
				return new Token('&');
		case '|':  // 与&同理
			if (readch('|'))  
				return Word.or;
			else
				return new Token('|');
		case '=':  // 与&同理
			if (readch('='))
				return Word.eq;
			else
				return new Token('=');
		case '!':  // 与&同理
			if (readch('='))
				return Word.ne;
			else
				return new Token('!');
		case '<':  // 与&同理
			if (readch('='))
				return Word.le;
			else
				return new Token('<');
		case '>':  // 与&同理
			if (readch('='))
				return Word.ge;
			else
				return new Token('>');
		}
		if (Character.isDigit(peek)) {  //调用Character类中的isDigit()方法    判断输入是否为数字0~9
			int v = 0;
			do {
				v = 10 * v + Character.digit(peek, 10);  // 把peek的值转换为十进制，依照十进制规则，越左边数值的幂次是其相邻右边数值幂次的十倍
				readch();
			} while (Character.isDigit(peek));  //处理连续输入的十进制数字
			if (peek != '.')  //若 连续的数字中没有小数点，则返回 一个 Num类型的对象，扫描结束。Num为Token的子类。
				return new Num(v);
			 //若数字中包含小数点，表明输入数字为小数。 一下为对小数部分的扫描
			float x = v; 
			float d = 10;
			for (;;) {
				readch();
				if (!Character.isDigit(peek))  //输入序列中下一个字符不是数字，则跳出循环
					break;
				x = x + Character.digit(peek, 10) / d;  // 十进制小数的处理。
				d = d * 10;
			}
			return new Real(x);  //返回一个Real类型的对象。Real类为Token类的子类。扫描结束。
		}
		//以字母开头的连续的串，如果后面包含数字，数字不会被抽取出来
		if (Character.isLetter(peek)) {  //判断输入字符是否为字母
			StringBuffer b = new StringBuffer();  //构造一个空字符缓冲区，初始容量为16个字符
			do {
				b.append(peek);  //将peek加进缓冲区
				readch();
			} while (Character.isLetterOrDigit(peek));  //将连续输入的字符加进一个缓冲区
			String s = b.toString();  // 将缓冲区b中的内容赋给一个String 变量s
			Word w = (Word) words.get(s);  //返回制定键s所映射的值 并强制类型转换 转换为Word类型 若 此映射不包含此键的映射 则返回null
			if (w != null)  // w不为null 表示 输入为一个关键字或保留值
				return w;  //结束扫描
			w = new Word(s, Tag.ID);  // 否则 输入为一个普通字符 标记为 ID
			words.put(s, w);  // 将输入存入哈希表
			return w;  //结束扫描
		}
		Token tok = new Token(peek);
		peek = ' ';
		return tok;
	}
	
	public void out() {
		System.out.println(words.size());  // 返回哈希表中键的数量
		
	}

	public char getPeek() {
		return peek;
	}

	public void setPeek(char peek) {
		this.peek = peek;
	}

}
