package inter;

import symbols.Type;
import lexer.Token;

public class Expr extends Node{

	//表示这个表达式的运算符和类型
	public Token op;
	public Type type;
	
	Expr(Token tok, Type p) { op = tok; type = p; }
	
	public Expr gen() { return this;}   //返回一个项，可成为一个三地址指令的右部，
	public Expr reduce() { return this;}//把一个表达式计算为一个单一的地址
	
	//为布尔表达式生成跳转代码
	public void jumping(int t, int f) {emitjumps(toString(),t,f);}
	
	public void emitjumps(String test, int t, int f){
		if(t!=0&&f!=0){
			emit("if"+test+" goto L"+t);
			emit("goto L"+f);
		}
		else if(t!=0) emit("if "+test+" goto L"+t);
		else if(f!=0) emit("iffalse "+test+" goto L"+f);   //  t是0，f不是0时，如果test不对，就goto L f
		else ;
	}
	public String toString() { return op.toString(); }
}
