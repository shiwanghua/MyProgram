package lexer;

//Word类继承了Token类
//包括一个公有的String数据类型，一个构造函数，一个toString函数
//定义了一些静态Word类型（自身）的常量

public class Word extends Token {
	public String lexeme = "";

	public Word(String s, int tag) {
		super(tag);
		lexeme = s;
	}

	public String toString() {
		return lexeme;           // 替换了父类的toString(只一个字符）
	}

	public static final Word and = new Word("&&", Tag.AND), or = new Word("||",Tag.OR), 
			eq = new Word("==", Tag.EQ), ne = new Word("!=", Tag.NE),
			le = new Word("<=", Tag.LE), ge = new Word(">=", Tag.GE),
			minus = new Word("minus", Tag.MINUS), True = new Word("true",Tag.TRUE),
			False = new Word("false", Tag.FALSE),temp = new Word("t", Tag.TEMP),
			Comment=new Word("//",Tag.COMMENT);
}
