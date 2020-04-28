package symbols;
//实现连接连接符号表
import inter.Id;

import java.util.Hashtable;

import lexer.Token;

public class Env {
	private Hashtable table;  // 存储符号表
	protected Env prev;  //指向上一级符号表
	
	public Env(Env n) { table = new Hashtable(); prev=n;}  //创建一个新的符号表
	
	public void put(Token w, Id i) {  //在当前符号表中写入一个条目
		table.put(w, i);
	}
	
	public Id get(Token w){  // 得到一个标识符的条目
		for(Env e=this; e!=null; e=e.prev){  //从当前符号开始搜索
			Id found = (Id)(e.table.get(w));  //找到后返回该条目
			if(found!=null) return found;  //找不到，返回null
		}
		return null;
	}
}
