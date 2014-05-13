// 日志发送器接口类
#ifndef _LOGSENDER_H
#define _LOGSENDER_H
#include <list>
using namespace std;
#include "data.h"
#include "except.h"
// 日志发送器接口类
class LogSender {
public:
	virtual ~LogSender (void) {}
	// 发送日志
	virtual void sendLog (list<MLogRec>& logs)
		throw (SendException) = 0;
};
#endif // _LOGSENDER_H
