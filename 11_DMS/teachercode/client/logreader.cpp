// 实现日志读取器类
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/stat.h>
#include <arpa/inet.h>
using namespace std;
#include "logreader.h"
// 构造器
LogReader::LogReader (const string& logFile,
	const string& loginsFile) :
	m_logFile (logFile),
	m_loginsFile (loginsFile) {}
// 读取日志
list<MLogRec>& LogReader::readLog (void)
	throw (ReadException) {
	cout << "读取日志开始..." << endl;
	try {
		// 备份日志文件
		backup ();
		// 读取登入文件
		readLoginsFile ();
		// 读取备份文件
		readBackupFile ();
		// 匹配登入登出
		match ();
		// 保存登入文件
		saveLoginsFile ();
	}
	catch (BackupException& ex) {
		throw ReadException ("备份错误");
	}
	catch (ReadException& ex) {
		throw ReadException ("读取错误");
	}
	catch (SaveException& ex) {
		throw ReadException ("保存错误");
	}
	catch (...) {
		throw ReadException ("未知错误");
	}
	cout << "读取日志完成。" << endl;
	return m_logs;
}
// 备份日志文件
void LogReader::backup (void)
	throw (BackupException) {
	cout << "备份文件开始..." << endl;
	// 生成备份文件名
	time_t now = time (NULL);
	tm* local = localtime (&now);
	ostringstream oss;
	oss << m_logFile << "." << setfill ('0')
		<< setw (4) << local->tm_year + 1900
		<< setw (2) << local->tm_mon + 1
		<< setw (2) << local->tm_mday
		<< setw (2) << local->tm_hour
		<< setw (2) << local->tm_min
		<< setw (2) << local->tm_sec;
	m_backupFile = oss.str ();
	// 生成备份指令
	string cmd ("./backup.sh ");
	cmd += m_logFile;
	cmd += " ";
	cmd += m_backupFile;
#ifdef _DEBUG
	cout << "备份指令：" << cmd << endl;
#endif
	// 执行备份指令
	int status = system (cmd.c_str ());
	if (WEXITSTATUS (status) == 1)
		throw BackupException ("拷贝错误");
	if (WEXITSTATUS (status) == 2)
		throw BackupException ("清空错误");	
	cout << "备份文件完成。" << endl;
}
// 读取登入文件
void LogReader::readLoginsFile (void)
	throw (ReadException) {
	cout << "读取登入文件开始..." << endl;
	ifstream ifs (m_loginsFile.c_str (),
		ios::binary);
	if (ifs) {
		LogRec log;
		while (ifs.read ((char*)&log,
			sizeof (log)))
			m_logins.push_back (log);
		if (! ifs.eof ())
			throw ReadException ();
		ifs.close ();
	}
#ifdef _DEBUG
	cout << "登入日志记录：" <<
		m_logins.size () << endl;
#endif
	cout << "读取登入文件完成。" << endl;
}
// 读取备份文件
void LogReader::readBackupFile (void)
	throw (ReadException) {
	cout << "读取备份文件开始..." << endl;
	// 打开备份文件
	ifstream ifs (m_backupFile.c_str (),
		ios::binary);
	if (! ifs)
		throw ReadException (
			"备份文件无法打开");
	// 计算记录条数
	struct stat st;
	if (stat (m_backupFile.c_str (), &st)==-1)
		throw ReadException (
			"无法获取备份文件大小");
	int records = st.st_size / 372;
	// 逐条读取记录
	for (int i = 0; i < records; ++i) {
		// 定位到记录开始
		ifs.seekg (i * 372, ios::beg);
		// 登录名
		char logname[32];
		ifs.read (logname, 32);
		// 登录进程ID
		ifs.seekg (36, ios::cur);
		pid_t pid;
		ifs.read ((char*)&pid, sizeof (pid));
		pid = ntohl (pid);
		// 登录类型：7-登入，8-登出
		short type;
		ifs.read ((char*)&type, sizeof(type));
		type = ntohs (type);
		// 登入登出时间
		ifs.seekg (6, ios::cur);
		long logtime;
		ifs.read ((char*)&logtime,
			sizeof (logtime));
		logtime = ntohl (logtime);
		// 登录IP长度
		ifs.seekg (28, ios::cur);
		short len;
		ifs.read ((char*)&len, sizeof (len));
		len = ntohs (len);
		// 登录IP或机器名
		char logip[len+1];
		ifs.read (logip, len);
		logip[len] = '\0';
		/*
#ifdef _DEBUG
		cout << logname << ','
			<< pid << ','
			<< type << ','
			<< logtime << ','
			<< len << ','
			<< logip << endl;
#endif
		*/
		// 登录名以.开头的用户是ssh/telnet
		// 软件建立的伪用户，丢弃之
		if (logname[0] != '.') {
			LogRec log;
			strcpy (log.logname, logname);
			strcpy (log.logip, logip);
			log.pid = pid;
			log.logtime = logtime;
			if (type == 7) // 登入记录
				m_logins.push_back (log);
			else
			if (type == 8) // 登出记录
				m_logouts.push_back (log);
		}
	}
	ifs.close ();
#ifdef _DEBUG
	cout << "登入日志记录："
		<< m_logins.size () << endl;
	cout << "登出日志记录："
		<< m_logouts.size () << endl;
#endif
	cout << "读取备份文件完成。" << endl;
}
// 匹配登入登出
void LogReader::match (void) {
	cout << "匹配登入登出开始..." << endl;
	// 对登出日志记录集中的每一条记录...
	for (list<LogRec>::iterator oit =
		m_logouts.begin ();
		oit != m_logouts.end (); ++oit)
		// 在登入日志记录集中查找匹配记录...
		for (list<LogRec>::iterator iit =
			m_logins.begin ();
			iit != m_logins.end (); ++iit) {
			// 登录名一致
			if (! strcmp (iit->logname,
				oit->logname) &&
				// 登录IP一致
				! strcmp (iit->logip,
				oit->logip) &&
				// 登录进程ID一致
				iit->pid == oit->pid) {
				MLogRec log = {"","",0,0,0,0};
				strcpy (log.logname,
					oit->logname);
				strcpy (log.logip,
					oit->logip);
				log.pid = oit->pid;
				log.logintime = iit->logtime;
				log.logouttime= oit->logtime;
				log.durations =
					log.logouttime -
						log.logintime;
				m_logs.push_back (log);
				// 从登入日志记录集中删除匹配
				// 的记录，剩余者存入登入文件
				m_logins.erase (iit);
				break;
			}
		}
#ifdef _DEBUG
	cout << "匹配日志记录："
		<< m_logs.size () << endl;
#endif
	cout << "匹配登入登出完成。" << endl;
}
// 保存登入文件
void LogReader::saveLoginsFile (void)
	throw (SaveException) {
	cout << "保存登入文件开始..." << endl;
	if (! m_logins.empty ()) {
		ofstream ofs (m_loginsFile.c_str (),
			ios::binary);
		if (! ofs)
			throw SaveException ();
		while (! m_logins.empty ()) {
			ofs.write ((const char*)
				&m_logins.front (),
				sizeof (LogRec));
			m_logins.pop_front ();
		}
		ofs.close ();
	}
	cout << "保存登入文件完成。" << endl;
}
