package inter;

import lexer.Lexer;

public class Node {   //   有两个子类：表达式结点Expr，语句结点Stmt
	int lexline = 0;  //
	
	Node() {lexline=Lexer.line;}  //及时获取结点所在行数
	
	void error(String s) { throw new Error("near line "+lexline+": "+s); }   // 输出错误信息
	
	// 下面的代码用于生成三地址代码
	static int labels = 0;   

	public int newlabel() { return ++labels;}
	
	public void emitlabel(int i) { System.out.print("L"+i+":"); }
	
	public void emit(String s) { System.out.println("\t"+s); }
}
