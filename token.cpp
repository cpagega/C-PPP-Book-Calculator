#include "token.h"
#include "lib.h"

const char number = '8';     // t.kind==number means that t is a number Token
const char quit = '`';       // t.kind==quit means that t is a quit Token
const char print = ';';      // t.kind==print means that t is a print Token
const string prompt = "> ";  // printed to console as an input prompt
const string result = "= ";  // used to indicate that what follows is a result
const char name = 'a';       // t.kind==a for variable Token
const char let = 'L';        // returned as a Token when declkey "let" matched in Token stream
const string declkey = "let"; // keyword for variable definition
const char root = 'R';		 // operator definition and switch case for root function.
const char power = '^';      // operator definition and switch case for power function.
const string exitkey = "exit";
const char help = '?';

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
	case root:
	case power:
	case help:
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
			if (s == exitkey) return Token(quit);
			return Token{ name,s };
		}

		error("Bad token");
	}
}