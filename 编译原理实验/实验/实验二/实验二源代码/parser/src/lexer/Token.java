package lexer;

public class Token {
	public final int tag;  //  一个Token一旦建立这个tag就不再变了
	public Token(int t) { tag = t; }
	public String toString() { return ""+(char)tag; }
}
