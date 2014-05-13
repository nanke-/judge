/*************************************************************************
	> File Name: executor.h
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月17日 星期四 13时00分54秒
 ************************************************************************/

#ifndef _EXECUTOR_H__
#define _EXECUTOR_H__

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

class Executor
{
public:
	Executor(char *configFile = NULL):m_time_used(0), m_memory_used(0), m_cpid(-1), m_time_limit(1000), m_memory_limit(32000), m_output_limit(32000), m_permited_file_cnt(0)
	{

		getConfigFile();
		read_config(m_config_path);
		if(configFile != NULL)
		{
			read_config(configFile);
		}

	};		

	bool operator <(const Executor &other)
	{
		return m_cpid < other.m_cpid;
	}
	
	void execute(void);
	int getCPid(void) const;
	int getTimeLimit(void)const;

private:
	int set_option(const xmlChar *key, const char *value);
	int read_config(char *config_file);
	void runProgram(void) const;
	void setLimits(void) const;
	void peekstring(int addr, char *str) const;
	int getMemoryUsed(void) const;
	void witnessSyscall(void);
	void result(int rs, int option) const;

	void forbidden_syscall_add(int syscall);
	int forbidden_syscall_check(int syscall);

	void permited_file_add(const char *value);
	int permited_file_check(const char *value);

	void getConfigFile(void);

	enum
	{
		RS_NML, 	//Normal
		RS_RTE, 	//Runtime Error
		RS_TLE, 	//Time Limit Exceed
		RS_MLE, 	//Memory Limit Exceed
		RS_OLE, 	//Output Limit Exceed
		RS_SYS, 	//Syscall Restricted
		RS_ECR, 	//Executer Error
	};

	int m_time_used;
	int m_memory_used;
	int m_cpid;
	int m_time_limit;
	int m_memory_limit;
	int m_output_limit;

	char m_program[128];
	char m_config_path[128];

	int m_permited_file_cnt;
	char *m_permited_file[1024];

	int m_forbidden_syscall[400];
};

extern Executor Exec;

void exe_timer(int signo);
void exe_setTimer(void);
int stringToInt(const char *str);

#endif