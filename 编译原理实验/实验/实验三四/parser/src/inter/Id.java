package inter;

import symbols.Type;
import lexer.Word;

public class Id extends Expr {   //  Id 对应一个叶子结点
	
	public int offset;   //保存这个标识符的相对地址
	public Id(Word id, Type p, int b) { super(id,p); offset=b; }
}
// 注：对应于一个类id的结点是一个叶子结点
