package parser;

import inter.Access;
import inter.And;
import inter.Arith;
import inter.Break;
import inter.Constant;
import inter.Do;
import inter.Else;
import inter.Expr;
import inter.For;
import inter.Id;
import inter.If;
import inter.Not;
import inter.Or;
import inter.Rel;
import inter.Seq;
import inter.Set;
import inter.SetElem;
import inter.Stmt;
import inter.Unary;
import inter.While;

import java.io.IOException;

import symbols.Array;
import symbols.Env;
import symbols.Type;
import lexer.Lexer;
import lexer.Tag;
import lexer.Token;
import lexer.Word;
import lexer.Num;
//语法分析器 ， 读入一个由词法单元构成的流，调用各种构造函数构建成一棵语法分析树
//对每个非终结符号都有个过程
public class Parser {

	   private Lexer lex;    // lexical analyzer for this parser
	   private Token look;   // lookahead tagen
	   Env top = null;       // current or top symbol table
	   int used = 0;         // storage used for declarations

	   public Parser(Lexer l) throws IOException { lex = l; move(); }

	   void move() throws IOException { look = lex.scan(); }

	   void error(String s) { throw new Error("near line "+lex.line+": "+s); }

	   void match(int t) throws IOException {
	      if( look.tag == t ) move();
	      else error("syntax error");
	   }

	   public void program() throws IOException {  // program -> block
	      Stmt s = block();   //  处理程序块，只有一个block，所以这里只能处理一个程序块，即第一个｛｝后面的代码将不会生成三地址代码了
	      
	      //   开始生成三地址代码
	      int begin = s.newlabel();  
	      int after = s.newlabel();
	      s.emitlabel(begin);      //   输出L1:
	      s.gen(begin, after);     //   唯一调用gen方法的地方！ 
	      s.emitlabel(after);      //   输出L2:
	   
	   }

	   Stmt block() throws IOException {  // block -> { decls stmts }
	      match('{');  Env savedEnv = top;  top = new Env(top);
	      decls(); Stmt s = stmts();
	      match('}');  top = savedEnv;
	      return s;   //   返回的是这个块里的所有语句了，s已经是一个Seq类，在progam里调用其gen()时会一个一个调用序列中所有语句的gen
	   }

	   void decls() throws IOException {
		   //  while循环以接受多次声明。一条语句只能声明一个对象。
	      while( look.tag == Tag.BASIC ) {   // D -> type ID ;    //   如果move得到的是Type常量Int，则其tag是Tag.BASIC  ! 这也就解决了我在实验一的报告里最后提出的问题
	         Type p = type(); Token tok = look; match(Tag.ID);  // match int 后面的变量名 
	         match(';');    //   一次声明结束
	         Id id = new Id((Word)tok, p, used);   //   新word，这个变量的类型，storage used for declarations，used应该是开始地址。。。
	         top.put( tok, id );    //  变量名（scan里扫描得到的新word），ID对象
	         used = used + p.width;
	      }
	   }

	   Type type() throws IOException {

	      Type p = (Type)look;            // expect look.tag == Tag.BASIC 
	      match(Tag.BASIC);
	      if( look.tag != '[' ) return p; // T -> basic    如果int后面没有[ ,返回这个Int常量
	      else return dims(p);            // return array type
	   }

	   Type dims(Type p) throws IOException {
	      match('[');  Token tok = look;  match(Tag.NUM);  match(']');
	      if( look.tag == '[' )
	      p = dims(p);
	      return new Array(((Num)tok).value, p);
	   }

	   //  程序前部分必须把需要声明的数据、局部变量全部声明完毕，下面是处理声明以后的代码
	   
	   Stmt stmts() throws IOException {
	      if ( look.tag == '}' ) return Stmt.Null;
	      else return new Seq(stmt(), stmts());
	   }

	   Stmt stmt() throws IOException {
	      Expr x;  Stmt s, s1, s2;
	      Stmt savedStmt;         // save enclosing loop for breaks

	      switch( look.tag ) {

	      case ';':
	         move();
	         return Stmt.Null;

	      case Tag.IF:
	         match(Tag.IF); match('('); x = bool(); match(')');
	         s1 = stmt();
	         if( look.tag != Tag.ELSE ) return new If(x, s1);
	         match(Tag.ELSE);
	         s2 = stmt();
	         return new Else(x, s1, s2);

	      case Tag.WHILE:
	         While whilenode = new While();
	         savedStmt = Stmt.Enclosing; Stmt.Enclosing = whilenode;
	         match(Tag.WHILE); match('('); x = bool(); match(')');
	         s1 = stmt();
	         whilenode.init(x, s1);
	         Stmt.Enclosing = savedStmt;  // reset Stmt.Enclosing
	         return whilenode;
	         
	      case Tag.FOR:
		    	 For fornode = new For();
		    	 savedStmt = Stmt.Enclosing; Stmt.Enclosing = fornode;
		    	 match(Tag.FOR);				match('('); 
		    	 Stmt fors1 = forassign(); 		match(';');
		    	 Expr forx = bool(); 			match(';'); 
		    	 Stmt fors2 = forassign();		match(')');
		    	 Stmt fors3 = stmt(); 
		    	 fornode.init(fors1, forx, fors2, fors3);
		    	 Stmt.Enclosing = savedStmt;
		    	 return fornode;

	      case Tag.DO:
	         Do donode = new Do();
	         savedStmt = Stmt.Enclosing; Stmt.Enclosing = donode;
	         match(Tag.DO);
	         s1 = stmt();
	         match(Tag.WHILE); match('('); x = bool(); match(')'); match(';');
	         donode.init(s1, x);
	         Stmt.Enclosing = savedStmt;  // reset Stmt.Enclosing
	         return donode;
	         
	      case Tag.BREAK:
	         match(Tag.BREAK); match(';');
	         return new Break();

	      case '{':
	         return block();

	      default:        //   默认是赋值语句！
	         return assign();
	      }
	   }
	   
	   Stmt forassign() throws IOException {
		      Stmt stmt;  Token t = look;
		      match(Tag.ID);
		      Id id = top.get(t);
		      if( id == null ) error(t.toString() + " undeclared");

		      if( look.tag == '=' ) {       // S -> id = E ;
		         move();  stmt = new Set(id, bool());
		      }
		      else {                        // S -> L = E ;
		         Access x = offset(id);
		         match('=');  stmt = new SetElem(x, bool());
		      }                  //   和assign()的唯一区别： 没有match‘;’
		      return stmt;
	   }

	   Stmt assign() throws IOException {
	      Stmt stmt;  Token t = look;
	      match(Tag.ID);
	      Id id = top.get(t);
	      if( id == null ) error(t.toString() + " undeclared");

	      if( look.tag == '=' ) {       // S -> id = E ;
	         move();  stmt = new Set(id, bool());   //   赋值给id对象
	      }
	      else {                        // S -> L = E ;
	         Access x = offset(id);
	         match('=');  stmt = new SetElem(x, bool());
	      }
	      match(';');
	      return stmt;
	   }

	   Expr bool() throws IOException {
	      Expr x = join();
	      while( look.tag == Tag.OR ) {
	         Token tok = look;  move();  x = new Or(tok, x, join());
	      }
	      return x;
	   }

	   Expr join() throws IOException {
	      Expr x = equality();
	      while( look.tag == Tag.AND ) {
	         Token tok = look;  move();  x = new And(tok, x, equality());
	      }
	      return x;
	   }

	   Expr equality() throws IOException {
	      Expr x = rel();
	      while( look.tag == Tag.EQ || look.tag == Tag.NE ) {
	         Token tok = look;  move();  x = new Rel(tok, x, rel());
	      }
	      return x;
	   }

	   Expr rel() throws IOException {
	      Expr x = expr();  //  如： if（a<b）  最终返回a 的ID对象 look.tag则是<
	      switch( look.tag ) {
	      case '<': case Tag.LE: case Tag.GE: case '>':
	         Token tok = look; //  保存这个比较符
	         move(); //得到比较符右边的对象
	         return new Rel(tok, x, expr());   // expr得到第二个比较数，Rel又调用其父类Logical的构造函数
	      default:
	         return x;
	      }
	   }

	   Expr expr() throws IOException {
	      Expr x = term();
	      while( look.tag == '+' || look.tag == '-' ) {
	         Token tok = look;  move();  x = new Arith(tok, x, term());
	      }
	      return x;
	   }

	   Expr term() throws IOException {
	      Expr x = unary();
	      while(look.tag == '*' || look.tag == '/' ) {
	         Token tok = look;  move();   x = new Arith(tok, x, unary());
	      }
	      return x;
	   }

	   Expr unary() throws IOException {
	      if( look.tag == '-' ) {
	         move();  return new Unary(Word.minus, unary());
	      }
	      else if( look.tag == '!' ) {
	         Token tok = look;  move();  return new Not(tok, unary());
	      }
	      else return factor();
	   }

	   Expr factor() throws IOException {
	      Expr x = null;
	      switch( look.tag ) {
	      case '(':
	         move(); x = bool(); match(')');
	         return x;
	      case Tag.NUM:
	         x = new Constant(look, Type.Int);    move(); return x;
	      case Tag.REAL:
	         x = new Constant(look, Type.Float);  move(); return x;
	      case Tag.TRUE:
	         x = Constant.True;                   move(); return x;
	      case Tag.FALSE:
	         x = Constant.False;                  move(); return x;
	      default:
	         error("syntax error");
	         return x;
	      case Tag.ID:     //  如： if（a<b）    这里look.tag就是a里的ID
	         String s = look.toString();
	         Id id = top.get(look);
	         if( id == null ) error(look.toString() + " undeclared");
	         move();//  如： look取到了 if（a<b）里的<
	         if( look.tag != '[' ) return id; //  不是数组值就返回这个Id
	         else return offset(id);
	      }
	   }

	   Access offset(Id a) throws IOException {   // I -> [E] | [E] I
	      Expr i; Expr w; Expr t1, t2; Expr loc;  // inherit id

	      Type type = a.type;
	      match('['); i = bool(); match(']');     // first index, I -> [ E ]
	      type = ((Array)type).of;
	      w = new Constant(type.width);
	      t1 = new Arith(new Token('*'), i, w);
	      loc = t1;
	      while( look.tag == '[' ) {      // multi-dimensional I -> [ E ] I
	         match('['); i = bool(); match(']');
	         type = ((Array)type).of;
	         w = new Constant(type.width);
	         t1 = new Arith(new Token('*'), i, w);
	         t2 = new Arith(new Token('+'), loc, t1);
	         loc = t2;
	      }

	      return new Access(a, loc, type);
	   }
	}
