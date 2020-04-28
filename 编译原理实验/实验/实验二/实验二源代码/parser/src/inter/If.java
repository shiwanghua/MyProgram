package inter;

import symbols.Type;

public class If extends Stmt {

   Expr expr; Stmt stmt;   //字段名字

   public If(Expr x, Stmt s) {
      expr = x;  stmt = s;
      if( expr.type != Type.Bool ) expr.error("boolean required in if");
   }

   public void gen(int b, int a) {
	   int label =newlabel();   // stmt代码的标号
	   expr.jumping(0,a);       // 如果expr的值为真，控制流必须穿越expr的代码，为假时转向标号a
	   emitlabel(label);
	   stmt.gen(label, a);
   }
   
   public void display(){
	   emit("stmt : if begin");
	   //expr
	   stmt.display();
	   emit("stmt : if end");
   }
}
