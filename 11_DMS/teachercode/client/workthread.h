#ifndef _WORKTHREAD_H
#define _WORKTHREAD_H

#include <QThread>
#include <QString>
#include <string>
using namespace std;

class WorkThread : public QThread
{
	Q_OBJECT
	public:
		void run(void);
		void update(const string& text);
	signals:
		void update(QString text);
};

#endif	// _WORKTHREAD_H
