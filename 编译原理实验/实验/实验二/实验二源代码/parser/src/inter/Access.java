package inter;// inter包中定义对应表达式和语句的类
         
import symbols.Type;
import lexer.Tag;
import lexer.Word;

public class Access extends Op{   //表示数组访问

	public Id array;
	public Expr index;
	public Access(Id a, Expr i, Type p){
		super(new Word("[]",Tag.INDEX),p);
		array=a; index = i;
	}
	
	//   生成“正常”代码
	public Expr gen() { return new Access(array, index.reduce(),type); }
	 
	public void jumping(int t, int f) {   //  生成跳转代码
		emitjumps(reduce().toString(),t,f);   //   把这个数组访问归约为一个临时变量，然后调用emitjumps
	}
	
	public String toString() {
		return array.toString()+" [ "+index.toString()+" ]";
	}
}
