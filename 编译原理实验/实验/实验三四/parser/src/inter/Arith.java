package inter;

import symbols.Type;
import lexer.Token;

public class Arith extends Op  {    //   实现双目运算符

	   public Expr expr1, expr2;

	   public Arith(Token tok, Expr x1, Expr x2)  {
	      super(tok, null); expr1 = x1; expr2 = x2;    //  tok是一个表示该类型的词法单元，null是类型占位符
	      type = Type.max(expr1.type, expr2.type);
	      if (type == null ) error("type error");
	   }

	   public Expr gen() {        // 把表达式的子表达式归约为地址，构造出三地址指令的右部
	      return new Arith(op, expr1.reduce(), expr2.reduce());
	   }

	   public String toString() {
	      return expr1.toString()+" "+op.toString()+" "+expr2.toString();
	   }
	}
