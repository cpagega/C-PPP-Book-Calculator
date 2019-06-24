

class Token {
public:
	char kind;        // what kind of token
	double value;     // for numbers: a value
	string name;      // for variable name 
	Token(char ch)    // make a Token 
		:kind(ch), value(0) { }
	Token(char ch, double val)
		:kind(ch), value(val) { }
	Token(char ch, string name)
		:kind(ch), name(name) { }

};

class Token_stream {
public:
	Token_stream();   // make a Token_stream that reads from cin
	Token get();      // get a Token (get() is defined elsewhere)
	void putback(Token t);    // put a Token back
	void ignore(char c);	  // discard characters up to and including a c
private:
	bool full;        // is there a Token in the buffer?
	Token buffer;     // here is where we keep a Token put back using putback()
};