package symbols;

import lexer.Tag;
import lexer.Word;

public class Type extends Word {
	public int width = 0;
	public Type(String s, int tag, int w) { super(s,tag); width=w; }
	public static final Type
		Int   = new Type( "int",   Tag.BASIC, 4 ),  
	    Float = new Type( "float", Tag.BASIC, 8 ),
	    Char  = new Type( "char",  Tag.BASIC, 1 ),
	    Bool  = new Type( "bool",  Tag.BASIC, 1 );     //四个基本类型对象
	
	public static boolean numeric(Type p)	{      //用于类型转换
		if(p==Type.Char||p==Type.Int||p==Type.Float) return true;
		else return false;
	}
	
	public static Type max(Type p1, Type p2){
		if(!numeric(p1)||!numeric(p2)) return null;  //   p1 p2 都可以转换时才不返回null，继续往下执行
		else if(p1==Type.Float||p2==Type.Float) return Type.Float;  
		else if(p1==Type.Int||p2==Type.Int) return Type.Int;
		else return Type.Char;   //  两个都是char时才返回 Char
	}
	
}
