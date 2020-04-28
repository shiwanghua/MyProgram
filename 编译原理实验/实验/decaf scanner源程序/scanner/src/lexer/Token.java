package lexer;

// Token类， 包括 一个int型常量(用于判断进入哪个case），构造函数和 toString函数

public class Token {

	public final int tag;

	public Token(int t) {
		tag = t;
	}

	public String toString() {
		return "" + (char) tag;
	}
}
