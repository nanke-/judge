/*
 * Author: Kinga
 * Date: 2013-09-13
 * 声明文件数据访问对象类
 */
#ifndef _FILEDAO_H
#define _FILEDAO_H

#include "logdao.h"
#include <fstream>
using namespace std;

// 文件数据访问对象类
class FileDao : public LogDao
{
	public:
		// 构造器
		FileDao(const string& path)
			throw (DBException);
		~FileDao(void);
		// 插入
		void insert(const MLogRec& log)
			throw (DBException);
	private:
		ofstream	m_ofs;		//文件输出流
};
#endif //_FILEDAO_H
