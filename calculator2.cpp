
//
// This is example code from Chapter 6.7 "Trying the second version" of
// "Software - Principles and Practice using C++" by Bjarne Stroustrup
//

/*
KNOWN BUG: 
1. After displaying help or a bad input, prompt will not return to >.
Work around is to enter an expression. It may not return the result but the prompt returns
and the calculator resumes normal operation for the next expression.

2. I included the capability to redefine variables but this also allows 
the predefined constants to be redefined. 
*/

/*
TODO:
Make the constants a little more constant
*/

#include "lib.h"
#include "token.cpp"
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
	// error("get: undefined variable ", s);  unneeded with current implementation
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
	// error("set: undefined variable ", s); unneeded with current implementation
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
	var_table.push_back(Variable(var, val));
	return val;
}

//-------------------------------------------------------------------------------

Token_stream ts;        // provides get(), putback(), and ignore() 

//------------------------------------------------------------------------------

double nroot(double n, int power)
/*
input: any number n, any positive int power
returns: only real roots of n
*/
{
	if (n < 0 && (power % 2) == 0) error("No real root");
	if (power < 0) error("Only positive powers allowed");
	double x = 1.0;
	double epsilon = .0001;
	double ans = 0.0;
	while (fabs(pow(ans, power) - n) > epsilon)
	{
		ans = x - (pow(x, power) - n) / (power * pow(x, power - 1));
		x = ans;
	}
	return ans;
}
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
	if (is_declared(var_name)) {
		set_value(var_name, d);
	}
	else
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
	{		
		if (is_declared(t.name)) {
			double d = get_value(t.name);   // get value of variable t.name
			return d;
		}
		else
			error("primary expected");
	}
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
			double ans = 1;
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


//--------------------------------------------------------------
double exponential()
{
	double left = factorial();
	Token t = ts.get();

	while (true) {
		switch (t.kind) {
		case root:
			left = nroot(left, factorial());
			t = ts.get();
			break;
		case power:
			left = pow(left, factorial());
			t = ts.get();
			break;
		default:
			ts.putback(t);
			return left;
		}
	}
}
//--------------------------------------------------------------



// deal with *, /, and %
double term()
{
	double left = exponential();
	Token t = ts.get();        // get the next token from token stream

	while (true) {
		switch (t.kind) {
		case '*':
			left *= exponential();
			t = ts.get();
			break;
		case '/':
		{
			double d = exponential();
			if (d == 0) error("divide by zero");
			left /= d;
			t = ts.get();
			break;
		}
		case '%':
		{
			double d = exponential();
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
	return "Welcome to calculator version 2.0 -- type '?' for more info.\n";
}

//------------------------------------------------------------------------
void clean_up_mess()
{
	ts.ignore(print);
}

//------------------------------------------------------------------------
void printhelp(); 
void calculate()		// expression evaluation loop
{
	while (cin) 
		try {
			cout << prompt;
			Token t = ts.get();
			while (t.kind == print) t = ts.get();    // first discard all "prints"
			if (t.kind == quit) return;
			if (t.kind == help){
				printhelp();
				t = ts.get();
			}
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
		define_name("k", 1.38064852e-23);   // Boltzmann
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

void printhelp()
{
	cout << "Type in an expression of any length and terminate with ';'\n";
	cout << "Example: 5! * (1+5);\n\n";
	cout << "Supported Operators: +,-,*,-,/, ^, n!, R, (), {}\n";
	cout << "nth roots use the R operator.\n";
	cout << "Example:  8R3; would be equivilent to the cube root of 8\n\n";
	cout << "Variables can be defined using the 'let' keyword\n";
	cout << "Example: let v1 = 5+5;\n\n";
	cout << "Stored constants:\n";
	cout << "pi = " << get_value("pi") << " PI\n";
	cout << "e = " << get_value("e") << " Euler\n";
	cout << "k = " << get_value("k") << " Boltzmann\n\n";
	cout << "Type 'exit' to exit\n";
	return;


}