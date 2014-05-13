/*************************************************************************
	> File Name: main.cpp
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月09日 星期三 11时14分43秒
 ************************************************************************/

#include <iostream>

#include "compiler.h"

using namespace std;

Compiler com("cpp", "/home/conlan/judge/compiler/1001.cpp", "1001");

int main(void)
{
	
	com.init();
	cout << "after init !! " << endl;
	com.compile();
	
	return 0;
}

