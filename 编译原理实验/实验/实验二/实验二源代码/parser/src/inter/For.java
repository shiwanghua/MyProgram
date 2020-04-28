package inter;
import symbols.Type;
public class For  extends Stmt {
	 Expr expr; 
	 Stmt stmt1,stmt2,stmt3;

	   public For() {expr=null; stmt1=stmt2=stmt3 = null; }

	   public void init(Expr x, Stmt s1,Stmt s2,Stmt s3) {
	      expr = x;  stmt1 = s1;stmt2=s2;stmt3=s3;
	      if( expr.type != Type.Bool ) 
	    	  expr.error("a boolean required in for");
	   }
	   public void gen(int b, int a) {    //  用于生成三地址代码
		   after=a;
		   expr.jumping(0, a);
		   int label=newlabel();   //用于stmt的标号
		   emitlabel(label);
		   stmt1.gen(label, b);   //  使While进入下一层迭代
		   emit(" goto L"+b);
	   }
	   
	   public void display() {
		   emit("stmt : for begin");
		   stmt1.display(); stmt2.display();  stmt3.display();
		   emit("stmt : for end");
	   }
}
