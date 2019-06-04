
//
// This is example code from Chapter 6.7 "Trying the second version" of
// "Software - Principles and Practice using C++" by Bjarne Stroustrup
//



#include "lib.h"
#include <cstdint>
//------------------------------------------------------------------------------
const char number = '8';     // t.kind==number means that t is a number Token
const char quit = 'x';       // t.kind==exit means that t is an exit Token
const char print = ';';      // t.kind==print means that t is a print Token
const string prompt = "> ";  // printed to console as an input prompt
const string result = "= ";  // used to indicate that what follows is a result
const char name = 'a';
const char let = 'L';
const string declkey = "let";

//------------------------------------------------------------------------------

class Variable {
public:
	string name;
	double value;
	Variable(string n, double v)
		:name(n), value(v) {}
};

vector<Variable> var_table;

//------------------------------------------------------------------------------

double get_value(string s)
// return the value of the Variable named s
{
	for (const Variable& v : var_table)
		if (v.name == s) return v.value;
	error("get: undefined variable ", s);
}

//------------------------------------------------------------------------------

void set_value(string s, double d)
// set the Variable named s to d
{
	for(Variable& v: var_table)
		if (v.name == s) {
			v.value = d;
			return;
		}
	error("set: undefined variable ", s);
}

//------------------------------------------------------------------------------

bool is_declared(string var)
// is var already in var_table?
{
	for (const Variable& v : var_table)
		if (v.name == var) return true;
	return false;
}

//------------------------------------------------------------------------------

double define_name(string var, double val)
// add (var,val) to var_table
{
	if (is_declared(var)) error(var, " declared twice");
	var_table.push_back(Variable(var, val));
	return val;
}

//-------------------------------------------------------------------------------

class Token{
public:
	char kind;        // what kind of token
	double value;     // for numbers: a value
	string name;      // for variable name 
	Token(char ch)    // make a Token from a char
		:kind(ch), value(0) { }
	Token(char ch, double val)    
		:kind(ch), value(val) { }
	Token(char ch, string name)
		:kind(ch), name(name) { }
};

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

// The constructor just sets full to indicate that the buffer is empty:
Token_stream::Token_stream()
	:full(false), buffer(0)    // no Token in buffer
{
}
//------------------------------------------------------------------------------
void Token_stream::ignore(char c)
// c represents the kind of Token
{
	// first look in buffer:
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	// now search input:
	char ch = 0;
	while (cin >> ch)
		if (ch == c) return;
}
//------------------------------------------------------------------------------

// The putback() member function puts its argument back into the Token_stream's buffer:
void Token_stream::putback(Token t)
{
	if (full) error("putback() into a full buffer");
	buffer = t;       // copy t to buffer
	full = true;      // buffer is now full
}

//------------------------------------------------------------------------------

Token Token_stream::get()
{
	if (full) {       // do we already have a Token ready?
		// remove token from buffer
		full = false;
		return buffer;
	}

	char ch;
	cin >> ch;    // note that >> skips whitespace (space, newline, tab, etc.)

	switch (ch) {
	case print:    
	case quit:
	case '(':
	case ')': 
	case '+': 
	case '-': 
	case '*':
	case '/':
	case '{': 
	case '}':
	case '!': 
	case '%':
	case '=':
		return Token(ch);        // let each character represent itself
	case '.':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	{
		cin.putback(ch);         // put digit back into the input stream
		double val;
		cin >> val;
		return Token(number, val);
	}
	default:
		if (isalpha(ch)) {
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) s += ch;
			cin.putback(ch);
			if (s == declkey) return Token(let);
			return Token{ name,s };
		}
		
		error("Bad token");
	}
}


//------------------------------------------------------------------------------

Token_stream ts;        // provides get() and putback() 

//------------------------------------------------------------------------------

double expression();    // declaration so that primary() can call expression()

//------------------------------------------------------------------------------

double declaration()
//assume we have seen "let"
// handle: name = expression
// declare a variable called "name" with the initial value "expression"
{
	Token t = ts.get();
	cout << "t.kind is: " << t.kind << "\n";
	if (t.kind != name) error("name expected in declaration");
	string var_name = t.name;

	Token t2 = ts.get();
	if (t2.kind != '=')error("= missing in declaration of ", var_name);

	double d = expression();
	define_name(var_name, d);
	return d;
}

double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration();
	default:
		ts.putback(t);
		return expression();
	}
}

// deal with numbers and parentheses
double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '{':
	{
		double d = expression();
		t = ts.get();
		if (t.kind != '}') error("'}' expected");
		return d;
	}
	case '(':    // handle '(' expression ')'
	{
		double d = expression();
		t = ts.get();
		if (t.kind != ')') error("')' expected");
		return d;
	}
	case number:
		return t.value;  // return the number's value
	case '-':
		return -primary();
	case '+':
		return primary();
	default:
		try {
			double d = get_value(t.name);
			return d;
		}
		catch (exception & e){
			cerr << e.what() << '\n';
	}
		
		//error("primary expected");
	}
}

//------------------------------------------------------------------------------

double factorial()
{
	double left = primary();
	Token t = ts.get();
	while (true) {
		switch (t.kind) {
		case '!':
		{
			uint64_t ans = 1;
			if (left == 0) {
				left = 1;
				t = ts.get();
				break;
			}
			else {
				for (int i = 1; i <= left; i++)
					ans *= i;
				left = ans;
				t = ts.get();
				break;
			}
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}


// deal with *, /, and %
double term()
{
	double left = factorial();
	Token t = ts.get();        // get the next token from token stream

	while (true) {
		switch (t.kind) {
		case '*':
			left *= factorial();
			t = ts.get();
			break;
		case '/':
		{
			double d = factorial();
			if (d == 0) error("divide by zero");
			left /= d;
			t = ts.get();
			break;
		}
		case '%':
		{
			double d = factorial();
			if (d == 0) error("divide by zero");
			left = fmod(left, d);
			t = ts.get();
			break;
		}
		default:
			ts.putback(t);     // put t back into the token stream
			return left;
		}
	}
}

//------------------------------------------------------------------------

// deal with + and -
double expression()
{
	double left = term();      // read and evaluate a Term
	Token t = ts.get();        // get the next token from token stream

	while (true) {
		switch (t.kind) {
		case '+':
			left += term();    // evaluate Term and add
			t = ts.get();
			break;
		case '-':
			left -= term();    // evaluate Term and subtract
			t = ts.get();
			break;
		default:
			ts.putback(t);     // put t back into the token stream
			return left;       // finally: no more + or -: return the answer
		}
	}
}

//------------------------------------------------------------------------

string greeting()
{
	string greeting = "Welcome to calculator version 2.0\n \
Please enter expressions using floating - point numbers\n \
This version supports operators + , -, *, / , x!(limited to 21!), % (remainder), and { () }\n \
Type ';' followed by return to calculate your entered expression\n \
Define variables using the 'let' keyword. (let var = 1)\n \
Type 'x' to exit (Note: Do not define a variable starting with 'x')\n";
	return greeting;
}

//------------------------------------------------------------------------
void clean_up_mess()
{
	ts.ignore(print);
}

//------------------------------------------------------------------------

void calculate()		// expression evaluation loop
{
	while (cin) 
		try {
			cout << prompt;
			Token t = ts.get();
			while (t.kind == print) t = ts.get();    // first discard all "prints"
			if (t.kind == quit) return;
			ts.putback(t);
			cout << result << statement() << '\n';
		}
		catch (exception & e) {
			cerr << e.what() << '\n';
			clean_up_mess();
		}
	
}




//-------------------------------------------------------------------------
int main()
{
	cout << greeting();
	try{
		// predefine names:
		define_name("pi", 3.1415926535);
		define_name("e", 2.7192818284);
		calculate();
		return 0;
	}
	catch (exception & e) {
		cerr << "error: " << e.what() << '\n';
		return 1;
	}
	catch (...) {
		cerr << "Oops: unknown exception!\n";
		return 2;
	}
}

//------------------------------------------------------------------------------