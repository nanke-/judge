/*
 * Author: Kinga
 * Date: 2013-09-12
 * 声明日志发送器
 */
#ifndef _CONSOLESENDER_H
#define _CONSOLESENDER_H
#include "logsender.h"

// 控制台日志发送器
class ConsoleSender : public LogSender
{
	public:
		// 发送日志
		void sendLog(list<MLogRec>& logs)
			throw (SendException);
};

#endif // _CONSOLESENDER_H
