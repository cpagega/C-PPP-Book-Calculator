#include <iostream>
#include "std_lib.h"

int main()
{
	char c = ' ';

	while (cin >> c)
	{
		switch (c)
		{
		case '1':
		{
			char b = c;
			cout << b;
			break;
		}
		default:
			cout << "Again\n";
			break;
		}
	}
	
}
