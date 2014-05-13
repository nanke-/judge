/*
 * Author: Kinga
 * Date: 2013-09-12
 * 声明日志发送器
 */
#ifndef _LOGSENDER_H
#define _LOGSENDER_H

#include <list>
using namespace std;
#include "data.h"
#include "except.h"

class LogSender
{
	public:
		virtual ~LogSender(void){}
		virtual void sendLog(list<MLogRec>& logs) 
			throw (SendException)= 0;
};

#endif // _LOGSENDER_H
