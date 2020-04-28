package lexer;

//Num类继承了Token类
//包括一个int型常量value，一个构造函数和 toString函数

public class Num extends Token {

	 public final int value;

	public Num(int v) {
		super(Tag.NUM);
		value = v;
	}

	public String toString() {
		return "" + value;
	}

}

