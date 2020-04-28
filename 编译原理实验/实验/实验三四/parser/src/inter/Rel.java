package inter;

import symbols.Array;
import symbols.Type;
import lexer.Token;

public class Rel extends Logical {

   public Rel(Token tok, Expr x1, Expr x2) { super(tok, x1, x2); }   //   父类构造函数里面的check调用的是Rel类中的check！

   public Type check(Type p1, Type p2) {
      if ( p1 instanceof Array || p2 instanceof Array ) return null; // 运算分量不能是数组类型
      else if( p1 == p2 ) return Type.Bool;   //   相同类型则说明可以用来比较
      else {
    	  error("type error: "+p1+" and "+p2+" cannot be used in operation "+op);
    	  return null;
      }
   }

   public void jumping(int t, int f) {
      Expr a = expr1.reduce();
      Expr b = expr2.reduce();
      String test = a.toString() + " " + op.toString() + " " + b.toString();
      emitjumps(test, t, f);
   }
}
