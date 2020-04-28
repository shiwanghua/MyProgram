package inter;

import symbols.Type;
import lexer.Num;
import lexer.Token;
import lexer.Word;

public class Constant extends Expr {

	 public Constant(Token tok, Type p) { super(tok, p); }   // 构造一个叶子结点
	   public Constant(int i) { super(new Num(i), Type.Int); } // 根据一个整数创建一个常量对象

	   public static final Constant
	      True  = new Constant(Word.True,  Type.Bool),
	      False = new Constant(Word.False, Type.Bool);

	   public void jumping(int t, int f) {
		   if(this==True&&t!=0)emit(" goto L"+t);   // 0是一个特殊标号
		   else if(this==False && f!=0)emit(" goto L"+f);
	   }
	}

