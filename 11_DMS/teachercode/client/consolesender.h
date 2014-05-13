// 声明控制台日志发送器类
#ifndef _CONSOLESENDER_H
#define _CONSOLESENDER_H
#include "logsender.h"
// 控制台日志发送器类
class ConsoleSender : public LogSender {
public:
	// 发送日志
	void sendLog (list<MLogRec>& logs)
		throw (SendException);
};
#endif // _CONSOLESENDER_H
