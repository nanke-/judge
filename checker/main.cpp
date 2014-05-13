#include <iostream>

#include "checker.h"

using namespace std;

int main(void)
{
	Checker check(string("ans"), string("stdout"));

	cout << check.checkDiff() << endl;
	return 0;
}