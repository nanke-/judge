/*************************************************************************
	> File Name: main.cpp
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月17日 星期四 13时01分58秒
 ************************************************************************/

#include <iostream>

#include "executor.h"

using namespace std;

Executor Exec("1001.xml");

int main(void)
{
	Exec.execute();
	return 0;
}
