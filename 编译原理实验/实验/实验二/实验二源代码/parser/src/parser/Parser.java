package parser;

import inter.Access;
import inter.And;
import inter.Arith;
import inter.Break;
import inter.Constant;
import inter.Do;
import inter.Else;
import inter.Expr;
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
import inter.For;
import java.io.IOException;

import symbols.Array;
import symbols.Env;
import symbols.Type;
import lexer.Lexer;
import lexer.Tag;
import lexer.Token;
import lexer.Word;
import lexer.Num;

// 语法分析器 ， 读入一个由词法单元构成的流，调用各种构造函数构建成一棵语法分析树
// 对每个非终结符号都有个过程
public class Parser {

	   private Lexer lex;    // lexical analyzer for this parser
	   private Token look;   // lookahead token
	   Env top = null;       // current or top symbol table
	   int used = 0;         // storage used for declarations

	   public Parser(Lexer l) throws IOException { lex = l; move(); }

	   void move() throws IOException { look = lex.scan(); }      //  扫描一次，往下看一次     move把读到的保留在look中

	   void error(String s) { throw new Error("near line "+Lexer.line+": "+s); }

	   void match(int t) throws IOException {   //  输入必须以{开头
	      if( look.tag == t ) move();    //如果匹配成功，继续往下看，扫描 
	      else error("syntax error");
	   }

	   public void program() throws IOException {  // program -> block
		  // build the syntax tree
	      Stmt s = block(true);     //  执行完这一句（对符号表的处理），整个程序就运行完了大部分了 
	      //一以下四行来自课本 ，用于生成中间代码
	      int begin= s.newlabel(); int after= s.newlabel();
	     s.emitlabel(begin);  //输出 “ L”+begin+“：”
	      s.gen(begin, after); 
	      s.emitlabel(after);  //输出“L”+after+“：”
	       //display the syntax tree
	      // only display the stmts, without expr 只呈现语句，不包括表达式，用assignment代替赋值
	      s.display();
	   }
       
	   Stmt block(boolean flag) throws IOException {  // block -> { decls stmts }
	      match('{');            //   ｛  是123  // 之前在Parser构造方法里已经move一次了，判断读到的是不是{ 
	      Env savedEnv = top;    //   指向前面的符号表的连接,有暂时保存的功能"temp"
	      top = new Env(top);    //   将top设置为刚刚创建的新符号表，并被连接到进入这个块之前一刻top的原值
	      decls(); 
	      Stmt s = stmts();
	      if(flag)   //   flag为true时代表这个块是最外层的块，不必再match最后的},因为match后还会要调用一次move
	      {
	    	  top=savedEnv;
	    	  return s;    
	      }
	      match('}');            //    } 是125
	      top = savedEnv;      //   恢复top的原值（进入块时的值）     
	      return s;
	   }

	   void decls() throws IOException {     // block 中会调用      用于处理声明变量的代码 
          //   BASIC
	      while( look.tag == Tag.BASIC) {  // 当look的tag等于257时 // D -> type ID ;
	    	 Token saved=look;
	         Type p = type(); Token tok = look; 
	         if(look.tag!=Tag.ID)  error(saved.toString()+"后面必须是"+"ID类型");
	         match(Tag.ID); match(';');
	         Id id = new Id((Word)tok, p, used);
	         top.put( tok, id );   //  程序中的声明被处理为符号表中有关标识符的条目，声明还可能生成在运行时刻为标识符保留存储空间的指令
	         used = used + p.width;
	      }
	   }

	   Type type() throws IOException {   //  判断类型

	      Type p = (Type)look;            // expect look.tag == Tag.BASIC   look被强制转换为Type类型，也就是BASIC
	      match(Tag.BASIC);
	      if( look.tag != '[' ) return p; // T -> basic
	      else return dims(p);            // return array type
	   }

	   Type dims(Type p) throws IOException {    //   处理数组类型
	      match('[');  Token tok = look;  match(Tag.NUM);  match(']');
	      if( look.tag == '[' )
	      p = dims(p);                    //   多级数组
	      return new Array(((Num)tok).value, p);
	   }

	   //  代码前部分必须把需要声明的数据、变量全部声明完毕，下面是处理除声明以外的代码
	   
	   Stmt stmts() throws IOException {
	      if ( look.tag == '}' ) return Stmt.Null;
	      else return new Seq(stmt(), stmts());
	   }

	   Stmt stmt() throws IOException {
	      Expr x;  Stmt s, s1, s2;
	      Stmt savedStmt;         // save enclosing loop for breaks

	      switch( look.tag ) {  //   对应非终结符号Stmt的各个产生式
	      case ';':
	         move();
	         return Stmt.Null;

	      case Tag.IF:
	         match(Tag.IF); 
	         if(look.tag!='(')
	        	 error("if后面必须有圆括号！");
	         match('('); x = bool(); match(')');   //    x是boolean表达式
	         s1 = stmt();                                         //   递归，实现嵌套功能
	         if( look.tag != Tag.ELSE ) return new If(x, s1);     //   实现else{}功能
	         match(Tag.ELSE);                            
	         s2 = stmt();
	         return new Else(x, s1, s2);

	      case Tag.WHILE:
	         While whilenode = new While();      //   创建while语句结点，可使内层break语句回指到它的外层循环语句
	         savedStmt = Stmt.Enclosing;         //   第一次运行时是空语句，如果有嵌套，则就是外一层的while结点了
	         Stmt.Enclosing = whilenode;         //   本层while结点
	         match(Tag.WHILE); 
	         if(look.tag!='(')
	        	 error("while后面必须有圆括号！");
	         match('('); x = bool(); match(')');
	         s1 = stmt();   //   递归
	         whilenode.init(x, s1);    //   最后才初始化最外层的while结点
	         Stmt.Enclosing = savedStmt;  // reset Stmt.Enclosing 回到上一层的while结点
	         return whilenode;

	      case Tag.FOR:                   //   识别for语句，和while，do语句类似
	    	  For fornode=new For();
	    	  savedStmt = Stmt.Enclosing; Stmt.Enclosing = fornode;
	    	  match(Tag.FOR); 
	    	  if(look.tag!='(')
		        	 error("for后面必须有圆括号！");
	    	  match('(');  s1 = stmt(); x = bool();  match(';');s2=stmt(); match(')');
	    	  s = stmt();    // 循环体
	    	  fornode.init(x,s1,s2,s);
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
	         return block(false);

	      default:
	         return assign();    //  默认是赋值语句
	      }
	   }

	   Stmt assign() throws IOException {
	      Stmt stmt;  Token t = look;
	      match(Tag.ID);
	      Id id = top.get(t);
	      if( id == null ) error(t.toString() + " undeclared");  // 报错：未声明的标识符

	      if( look.tag == '=' ) {       // S -> id = E ;
	         move();  stmt = new Set(id, bool());
	      }
	      else {                        // S -> L = E ;
	         Access x = offset(id);
	         match('=');  stmt = new SetElem(x, bool());
	      }
	      if(look.tag!=')')         // 满足for循环中的每次循环结束运行的代码（不要分号的赋值）
	      match(';');
	      return stmt;
	   }

	   //   一下九个方法全是与boolean表达式有关的
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
	      Expr x = expr();
	      switch( look.tag ) {
	      case '<': case Tag.LE: case Tag.GE: case '>':
	         Token tok = look;  move();  return new Rel(tok, x, expr());
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
	      case Tag.ID:
	         String s = look.toString();
	         Id id = top.get(look);
	         if( id == null ) error(look.toString() + " undeclared");
	         move();
	         if( look.tag != '[' ) return id;
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
