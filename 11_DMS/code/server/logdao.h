/*
 * Author: Kinga
 * Date: 2013-09-13
 * 数据访问对象接口类
 */
#ifndef _LOGDAO_H
#define _LOGDAO_H

#include "except.h"
#include "data.h"

// 数据访问对象接口类
class LogDao
{
	public:
		virtual ~LogDao(void){}
		//插入
		virtual void insert(const MLogRec& log)
			throw (DBException) = 0;
};
#endif //_LOGDAO_H
