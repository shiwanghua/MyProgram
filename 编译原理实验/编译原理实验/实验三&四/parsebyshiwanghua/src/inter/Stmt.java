package inter;

public class Stmt extends Node {
	   public Stmt() { }   //  子类中会重写

	   public static Stmt Null = new Stmt();   // 空语句序列

	   // 以下四行处理三地址代码的生成
	   public void gen(int b, int a) {} // called with labels begin and after：  b 语句开始处的标号   a 语句的下一条指令的标号

	   int after = 0;                   // saves label after
	   public static Stmt Enclosing = Stmt.Null;  // used for break stmts
	   
	   public void display() {}
	}