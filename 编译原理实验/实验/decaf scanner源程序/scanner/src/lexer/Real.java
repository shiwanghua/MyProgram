package lexer;
//Real类，用于处理浮点数
//包括一个float类型的常量，一个构造函数（调用了父类的构造函数，并以Tag类的REAL为参数），一个toString函数

public class Real extends Token {
	public final float value;

	public Real(float v) {
		super(Tag.REAL);
		value = v;
	}

	public String toString() {
		return "" + value;
	}
}
