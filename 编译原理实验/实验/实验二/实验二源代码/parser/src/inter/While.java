package inter;

import symbols.Type;

public class While extends Stmt {

   Expr expr; Stmt stmt;

   public While() { expr = null; stmt = null; }

   public void init(Expr x, Stmt s) {
      expr = x;  stmt = s;
      if( expr.type != Type.Bool ) expr.error("boolean required in while");
   }
   public void gen(int b, int a) {    //  用于生成三地址代码
	   after=a;
	   expr.jumping(0, a);
	   int label=newlabel();   //用于stmt的标号
	   emitlabel(label);
	   stmt.gen(label, b);   //  使While进入下一层迭代
	   emit("goto L"+b);
   }
   
   public void display() {
	   emit("stmt : while begin");
	   stmt.display();
	   emit("stmt : while end");
   }
}
