package inter;

import symbols.Type;

public class While extends Stmt {

   Expr expr; Stmt stmt;

   public While() { expr = null; stmt = null; }

   public void init(Expr x, Stmt s) {
      expr = x;  stmt = s;
      if( expr.type != Type.Bool ) expr.error("boolean required in while");
   }
   public void gen(int b, int a) {
	   after = a;            // save label a
      expr.jumping(0, a);       //  "iffalse "+expr.op.toString()+" goto L"+f  如果表达式错误 ， 跳到a
      int label = newlabel();   // label for stmt
      emitlabel(label);         //   输出Llabel： 
      stmt.gen(label, b);
      emit("goto L" + b);
   }
   
   public void display() {
	   emit("stmt : while begin");
	   stmt.display();
	   emit("stmt : while end");
   }
}
