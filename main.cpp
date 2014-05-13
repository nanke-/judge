/*************************************************************************
	> File Name: main.cpp
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月18日 星期五 21时12分08秒
 ************************************************************************/

#include <iostream>

#include "compiler.h"
#include "executor.h"
#include "checker.h"
#include "except.h"

using namespace std;


Executor Exec("data/1001/1001.xml");
Compiler com("cpp", "/home/conlan/judge/1001.cpp", "1001");
Checker checker(string("data/1001/ans"), string("stdout"));

int main(void)
{
	try
	{
		log_open("run.log");
		FM_LOG_DEBUG("opened");
		com.init();
		cout << "after init!!" << endl;
		com.compile();
		cout << com.getResult() << endl;
		if(string("Compile Error") == com.getResult())
		{
			return 0;
		}
		cout << "after compile" << endl;

		Exec.execute();
		cout << "after execute" << endl;

		int result = checker.checkDiff();
		if(result == 0)
		{
			cout << "AC" << endl;
		}
		else if(result == 1)
		{
			cout << "PE" << endl;
		}
		else if(result == 2)
		{
			cout << "WA" << endl;
		}
	}
	// catch(exception &ex)
	// {
	// 	cout << ex.what() << endl;
	// }
	catch(JudgeException &ex)
	{
		cout << ex.what() << endl;
	}
	return 0;
}	



