package inter;

import symbols.Type;
import lexer.Token;

public class Logical extends Expr{
	public Expr expr1, expr2;
	Logical(Token tok, Expr x1, Expr x2){
		super(tok,null);   //  如： tok是一个<比较符
		expr1 = x1; expr2 = x2;
		type = check(expr1.type, expr2.type);   //   如果是在子类Rel构造函数里调用这个父类构造函数，则这个check是调用的子类的check！
		if(type==null) error("type error");
	}
	
	public Type check(Type p1, Type p2){  //   保证expr1，expr2都是boolean类型
		if(p1==Type.Bool&&p2==Type.Bool) return Type.Bool;
		else return null;
	}
	
	public Expr gen(){
		int f = newlabel(); int a = newlabel();
		Temp temp = new Temp(type);
		this.jumping(0,f);
		emit(temp.toString()+" = true");
		emit("goto L"+a);
		emitlabel(f);emit(temp.toString()+" = false");  //把false赋给temp
		emitlabel(a);   // 生成标号a
		return temp;
	}
	 public String toString() {
	      return expr1.toString()+" "+op.toString()+" "+expr2.toString();
	 }
}
