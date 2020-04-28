package inter;

public class Break extends Stmt {
	   Stmt stmt;   //   保存外围语句的构造

	   public Break() {
	      if( Stmt.Enclosing == Stmt.Null ) error("unenclosed break");   //   Stmt.Enclosing 表示外围构造对应的语法树结点
	      stmt = Stmt.Enclosing;
	   }

	   public void gen(int b, int a) {
		   emit("goto L"+stmt.after);   //   跳转到stmt的代码之后的第一条指令
	   }
	   
	   public void display(){
		   emit(" break ");
	   }
}
