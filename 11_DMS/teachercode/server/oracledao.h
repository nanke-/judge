// 声明Oracle数据访问对象类
#ifndef _ORACLEDAO_H
#define _ORACLEDAO_H
#include <fstream>
using namespace std;
#include "logdao.h"
// Oracle数据访问对象类
class OracleDao : public LogDao {
public:
	// 构造器
	OracleDao (const string& username,
		const string& password)
		throw (DBException);
	~OracleDao (void);
	// 插入
	void insert (const MLogRec& log)
		throw (DBException);
};
#endif // _ORACLEDAO_H
