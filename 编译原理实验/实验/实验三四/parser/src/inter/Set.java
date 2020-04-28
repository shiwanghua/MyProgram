package inter;

import symbols.Type;

public class Set extends Stmt {    //   与赋值有关的类

   public Id id; public Expr expr;

   public Set(Id i, Expr x) {
      id = i; expr = x;
      if ( check(id.type, expr.type) == null ) error("type error");
      if(checkSameType(id.type,expr.type)==null) error("assignment type error: "+"can't assign a "+expr.type+" to a "+id.type);
   }

   public Type check(Type p1, Type p2) {
      if ( Type.numeric(p1) && Type.numeric(p2) ) return p2;         //不能判断赋值左部和右部类型是否相同，只能判断是不是四种基本数据类型
      else if ( p1 == Type.Bool && p2 == Type.Bool ) return p2;      //   这里不包括对数组的赋值
      else return null;
   }

   public Type checkSameType(Type p1,Type p2)
   {
	   if(p1==p2)   //   或者 p1.lexeme.equals(p2.lexeme)
	   return p1;
	   else return null;
   }
   public void gen(int b, int a) {
      emit( id.toString() + " = " + expr.gen().toString() );
   }
   
   public void display(){
	   emit(" assignment ");    //   输出“assignment”
   }
}