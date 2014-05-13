/*
 * Author: Kinga
 * Date: 2013-09-12
 * 实现控制台日志发送器
 */
#include <iostream>
#include "consolesender.h"
using namespace std;

// 控制台日志发送器
void ConsoleSender::sendLog(list<MLogRec>& logs)
	throw (SendException)
{
	cout << "发送日志开始..." << endl;
	// more codes
	
	for(list<MLogRec>::iterator it = logs.begin();
			it != logs.end(); ++it)
	{
		cout << *it << endl;
	}
	
	cout << "发送日志完成。" << endl;
}
