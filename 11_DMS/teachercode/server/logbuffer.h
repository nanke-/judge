// 声明日志缓冲器类
#ifndef _LOGBUFFER_H
#define _LOGBUFFER_H
#include <pthread.h>
#include <list>
using namespace std;
#include "data.h"
// 日志缓冲器类
class LogBuffer {
public:
	// 构造器
	LogBuffer (void);
	// 压入日志记录
	LogBuffer& operator<< (
		const MLogRec& log);
	// 弹出日志记录
	LogBuffer& operator>> (MLogRec& log);
private:
	pthread_mutex_t m_mutex; // 同步互斥量
	pthread_cond_t m_cond;	//同步条件变量
	list<MLogRec>   m_logs;  // 日志记录集
};
extern LogBuffer g_logBuffer;
#endif // _LOGBUFFER_H
