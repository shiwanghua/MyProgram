package symbols;       //   实现 符号表和类型

import lexer.Tag;

public class Array extends Type{      //  Array是一种构造类型
	public Type of;
	public int size = 1;
	public Array(int sz, Type p){
		super("[]", Tag.INDEX, sz*p.width); size = sz; of = p;  //266  // sz*p.width用于计算地址
	}
	public String toString() { return "["+size+"]"+of.toString(); }
}
