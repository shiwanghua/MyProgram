package symbols;

import inter.Id;

import java.util.Hashtable;

import lexer.Token;

public class Env {      //  把字符串词法单元映射为类id的对象
	private Hashtable table;
	protected Env prev;
	
	public Env(Env n) { table = new Hashtable(); prev=n;}
	
	public void put(Token w, Id i) {
		table.put(w, i);
	}
	
	public Id get(Token w){
		for(Env e=this; e!=null; e=e.prev){
			Id found = (Id)(e.table.get(w));
			if(found!=null) return found;
		}
		return null;
	}
}
