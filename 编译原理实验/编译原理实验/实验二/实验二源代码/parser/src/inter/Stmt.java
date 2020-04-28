package inter;
//节点的一个子类  语句

public class Stmt extends Node {

   public Stmt() { }

   public static Stmt Null = new Stmt();

   public void gen(int b, int a) {} // called with labels begin and after

   int after = 0;                // saves label after  默认为 public
   public static Stmt Enclosing = Stmt.Null;  // used for break stmts
   
   public void display() {}   //显示
}
