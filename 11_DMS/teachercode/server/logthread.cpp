// 实现线程类
#include <pthread.h>
#include <iostream>
using namespace std;
#include "logthread.h"
// 启动线程
void LogThread::start (void)
	throw (ThreadException) {
	cout << "启动线程开始..." << endl;
	pthread_t tid;
	if (pthread_create (&tid, NULL, run,
		this))
		throw ThreadException (
			"启动线程失败");
	cout << "启动线程完成。" << endl;
}
// 线程过程
void* LogThread::run (void* arg) {
	pthread_detach (pthread_self ());
	static_cast<LogThread*> (arg)->run ();
	return NULL;
}
