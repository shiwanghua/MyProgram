package inter;

import symbols.Type;

public class For extends Stmt {
	Expr expr; Stmt stmt1; Stmt stmt2; Stmt stmt3; 
	
	public For() { expr = null; stmt1 = null; stmt2 = null; stmt3 = null; }
	
	public void init(Stmt s1, Expr x, Stmt s2, Stmt s3) {   //   s3是循环体
		expr = x; stmt1 = s1; stmt2 = s2; stmt3 = s3;
		if( expr.type != Type.Bool ) expr.error("boolean required in for");
	}
	
	public void gen(int b, int a) {
		   int label1=newlabel();   //  用于stmt1的标号
		   stmt1.gen(b, label1);
		   
		   emitlabel(label1);     // 输出Llabel1： 
		   after=a;
		   expr.jumping(0, a);    //  "iffalse "+expr+" goto L"+a  如果表达式错误 ， 跳到a
		   
		   int label2=newlabel();   //用于stmt2的标号
		   emitlabel(label2);     // 输出Llabel2： 
		   int label3=newlabel();
		   stmt3.gen(label2, label3); 
		   
		   emitlabel(label3);
	       stmt2.gen(label3, label1);   //  使for进入下一层迭代
		   emit(" goto L"+label1);     //  输出“ goto L”+b
	}

	 public void display() {
		   emit("stmt : for begin");
		   stmt1.display(); stmt2.display();  stmt3.display();
		   emit("stmt : for end");
	   }
}
