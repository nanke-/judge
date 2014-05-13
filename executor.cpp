/*************************************************************************
	> File Name: executor.cpp
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月17日 星期四 13时01分12秒
 ************************************************************************/


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/ptrace.h>

#include "executor.h"
#include "logger.h"
#include "except.h"

#include <iostream>

using namespace std;
void Executor::getConfigFile(void)
{
	sprintf(m_config_path, "/proc/%d/exe", getpid());
	m_config_path[readlink(m_config_path, m_config_path, 1023)] = '\0';
	strrchr(m_config_path, '/')[1] = 0;
	sprintf(&m_config_path[strlen(m_config_path)], "executor.xml");
}

void Executor::permited_file_add(const char *value)
{
	int lenv = strlen(value);
	m_permited_file[m_permited_file_cnt] = (char *)malloc((lenv + 1) * sizeof(char));
	strcpy(m_permited_file[m_permited_file_cnt], value);
	m_permited_file_cnt++;
}

void Executor::forbidden_syscall_add(int syscall)
{
	if(syscall >= 400)
	{
		return;
	}
	m_forbidden_syscall[syscall] = 1;
}

int Executor::forbidden_syscall_check(int syscall)
{
	if(syscall >= 400)
		return 1;
	return m_forbidden_syscall[syscall];
}

int Executor::permited_file_check(const char *value)
{
	for(int i = 0; i < m_permited_file_cnt; ++i)
	{
		if(strcmp(value, m_permited_file[i]) == 0)
		{
			return 1;
		}
	}
	return 0;
}


int stringToInt(const char *str)
{
	int num;
	sscanf(str, "%d", &num);
	return num;
}

int Executor::set_option(const xmlChar *key, const char *value)
{
	if(xmlStrcmp(key, (const xmlChar *)"program") == 0)
	{
		strcpy(m_program, value);
	}
	else if(xmlStrcmp(key, (const xmlChar*)"time_limit") == 0)
	{
		m_time_limit = stringToInt(value);
	}
	else if(xmlStrcmp(key, (const xmlChar *)"memory_limit") == 0)
	{
		m_memory_limit = stringToInt(value);
	}
	else if(xmlStrcmp(key, (const xmlChar *)"output_limit") == 0)
	{
		m_output_limit = stringToInt(value);
	}
	else if(xmlStrcmp(key, (const xmlChar *)"stack_limit") == 0)
	{
		m_stack_limit = stringToInt(value);
	}
	else if(xmlStrcmp(key, (const xmlChar *)"file") == 0)
	{
		permited_file_add(value);
	}
	else if(xmlStrcmp(key, (const xmlChar *)"syscall") == 0)
	{
		forbidden_syscall_add(stringToInt(value));
	}
	else
	{
		return 1;
	}
	return 0;
}

int Executor::read_config(char *config_file)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	xmlChar *value;

	doc = xmlParseFile(config_file);
	if(doc == NULL)
	{
		return 1;
	}
	cur = xmlDocGetRootElement(doc);

	if(xmlStrcmp(cur->name, (const xmlChar*)"document") != 0)
	{
		xmlFreeDoc(doc);
		return 1;
	}
	for(cur = cur->xmlChildrenNode; cur != NULL; cur = cur->next)
	{
		if(cur->type == 1)
		{
			value = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			set_option(cur->name, (const char *)value);
			xmlFree(value);
		}
	}

	xmlFreeDoc(doc);
	return 0;
}

void Executor::setLimits(void) const
{
	struct rlimit rl_time_limit, rl_output_limit, rl_stack_limit;
	//Time limit
	if(m_time_limit > 0)
	{
		rl_time_limit.rlim_max = rl_time_limit.rlim_cur = (m_time_limit + 999) / 1000 + 1;
		if(setrlimit(RLIMIT_CPU, &rl_time_limit) < 0)
		{
			FM_LOG_WARNING("set CPU limit fail\n");
			throw ExecutorException(string("set CPU limit fail"));
		}
	}

	//stack limit
	if(m_stack_limit > 0)
	{
		FM_LOG_DEBUG("m_stack_limit = %d\n", m_stack_limit);
		rl_stack_limit.rlim_max = rl_stack_limit.rlim_cur = m_stack_limit * 1024;
		if(setrlimit(RLIMIT_STACK, &rl_stack_limit) < 0)
		{
			FM_LOG_WARNING("set Stack limit fail\n");
			throw ExecutorException(string("set Stack limit fail"));
		}
		FM_LOG_DEBUG("set Stack limit : m_stack_limit = %d", m_stack_limit);
	}

	if(m_output_limit > 0)
	{
		//Set Output File Size Limit
		rl_output_limit.rlim_max = rl_output_limit.rlim_cur = m_output_limit;
		if(setrlimit(RLIMIT_FSIZE, &rl_output_limit) < 0)
		{
			FM_LOG_WARNING("set output limit fail\n");
			throw ExecutorException(string("set output limit fail"));
		}
	}

}

void Executor::runProgram(void) const
{

	//Set ptrace
	if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0)
	{
		FM_LOG_WARNING("set ptrace fail\n");
		throw ExecutorException(string("set ptrace fail"));
	}

	execl(m_program, m_program, NULL);
}

void Executor::result(int rs, int option) const
{
	ptrace(PTRACE_KILL, m_cpid, NULL, NULL);
	switch(rs)
	{
		case RS_NML:
			printf("Exe succeed\n");
			break;
		case RS_RTE:
			printf("Runtime Error\n");
			break;
		case RS_TLE:
			printf("Time Limit Exceeded\n");
			break;
		case RS_MLE:
			printf("Memory Limit Exceeded\n");
			break;
		case RS_OLE:
			printf("Output Limit Exceeded\n");
			break;
		case RS_SYS:
			printf("Syscall Restricted\n");
			break;
		case RS_ECR:
			printf("Execute Error\n");
			break;
		default:
		break;
	}
	FM_LOG_DEBUG("signal: %d time: %d memory: %d\n",option, m_time_used, m_memory_used);
	//exit(0);
}

void Executor::peekstring(int addr, char *str) const
{
	char *pstr = str;
	union peeker
	{
		long val;
		char chars[sizeof(long)];
	}data;
	int k = 1;
	for(int i = 0; k; ++i)
	{
		data.val = ptrace(PTRACE_PEEKDATA, m_cpid, addr + i * 4, NULL);
		memcpy(pstr, data.chars, sizeof(long));
		for(int j = 0; j < sizeof(long); ++j)
		{
			if(pstr[j] == 0)
			{
				k = 0;
				break;
			}
		}
		pstr += sizeof(long);
	}
	
}

int  Executor::getMemoryUsed(void) const
{
	FILE *fps;
	char ps[32];
	int memory;

	sprintf(ps, "/proc/%d/statm", m_cpid);
	fps = fopen(ps, "r");

	if(NULL == fps)
	{
		FM_LOG_WARNING("open file fail: %s\n", ps);
		throw ExecutorException(string("open file fail: ") + string(ps));
	}

	for(int i = 0; i < 6; ++i)
	{
		fscanf(fps, "%d", &memory);
	}

	fclose(fps);

	int pagesize = getpagesize() / 1024;
	
	return memory *= pagesize;
}

int Executor::witnessSyscall(void)
{
	struct user_regs_struct reg;
	int syscall;
	static int executed = 0;
	int forbidden = 0;

	if(ptrace(PTRACE_GETREGS, m_cpid, NULL, &reg) < 0)
	{
		FM_LOG_WARNING("ptrace_getregs fail\n");
		throw ExecutorException(string("PTRACE_GETREGS fail"));
	}

	#ifdef __i386__
	syscall = reg.orig_eax;
	#else
	syscall = reg.orig.rax;
	#endif


	if(forbidden_syscall_check(syscall))
		forbidden = 1;

	if(syscall == SYS_execve)
	{
		if(!executed)
			executed = 1;
		else 
			forbidden = 1;
	}

	if(syscall == SYS_open)
	{
		char *filename = (char*)malloc(1024);
		peekstring(reg.ebx, filename);
		int flag = reg.ecx;

		if(!permited_file_check(filename))
		{
			forbidden = 1;
		}

		free(filename);
	}

	if(syscall == SYS_execve || syscall == SYS_brk
		|| syscall == SYS_mmap || syscall == SYS_mmap2
		|| syscall == SYS_munmap)
	{
		int memory_current = 0;
		try
		{
			memory_current = getMemoryUsed();
		}
		catch(JudgeException &ex)
		{
			throw;
		}
		FM_LOG_TRACE("current memory used = %d\n", memory_current);


		if(memory_current > m_memory_used)
		{
			m_memory_used = memory_current;
		}
		if(m_memory_limit > 0 && m_memory_used > m_memory_limit)
		{
			FM_LOG_TRACE("Memory Limit Exceeded\n");
			result(RS_MLE, 0);
		}
	}

	if(forbidden)
	{
		FM_LOG_TRACE("Syscall Restricted\n");
		result(RS_SYS,syscall);
	}
	return forbidden;
}

int Executor::getCPid(void) const
{
	return m_cpid;
}

int Executor::getTimeLimit(void) const
{
	return m_time_limit;
}

void timer(int signo)
{
	kill(Exec.getCPid(), SIGUSR1);
	alarm(1);
}

void setTimer()
{
	//setitimer()

	if(Exec.getTimeLimit() > 0)
	{
		if(signal(SIGALRM, timer) == SIG_ERR)
		{
			FM_LOG_WARNING("cannot set Timer\n");
			throw ExecutorException(string("cannot set Timer"));
		}
		alarm(1);	
	}
}

void Executor::set_security_option(void) const
{
	//必须先getpwnam, 再chroot, 再setuid
	//先setuid 就无法getpwnam, 先setuid 就没法chroot

	struct passwd *nobody = getpwnam("nobody");
	if(NULL == nobody)
	{
		FM_LOG_WARNING("no user named 'nobody'? %d: %s\n", errno, strerror(errno));
		throw ExecutorException(string("no user named 'nobody'"));
	}
	// temp_dir 

	// if(EXIT_SUCCESS != chdir(m_temp_dir.c_str())
	// {
	// 	printf("chdir(%s) failed, %d: %s\n", m_temp_dir.c_str(), errno, strerror(errno));
	// 	exit(1);
	// }

	char cwd[1024], *tmp = getcwd(cwd, 1024);
	if(NULL == tmp)
	{
		FM_LOG_WARNING("getcwd failed, %d %s\n", errno, strerror(errno));
		throw ExecutorException(string("getcwd failed, ") + string(strerror(errno)));
	}

	FM_LOG_DEBUG("cwd: %s\n", cwd);

	if(EXIT_SUCCESS != chroot(cwd))
	{
		FM_LOG_DEBUG("chroot(%s) failed, %d %s\n", cwd, errno, strerror(errno));
		throw ExecutorException(string("chroot fail"));
	}
	getcwd(cwd, 1024);
	FM_LOG_DEBUG("after chroot , cwd: %s\n", cwd);
	//setuid
	if(EXIT_SUCCESS != setuid(nobody->pw_uid))
	{
		FM_LOG_WARNING("setuid(%d) failed, %d %s\n", nobody->pw_uid, errno, strerror(errno));
		throw ExecutorException(string("setuid fail"));
	}
	FM_LOG_TRACE("set_security_option ok\n");
}


void Executor::redirect_io(void)const
{
	//Redirect IO
	freopen("data/1001/stdin", "r", stdin);
	freopen("stdout", "w", stdout);
	freopen("stderr", "w", stderr);

}

void Executor::execute(void)
{
	if((m_cpid = fork()) == 0)
	{
		try
		{
			redirect_io();
			set_security_option();
			setLimits();
			runProgram();
		}
		catch(JudgeException &ex)
		{
			cout  << ex.what() << endl;
			exit(1);
		}

		exit(0);
	}

	try
	{
		setTimer();		
	}
	catch(JudgeException &ex)
	{
		throw;
	}

	struct rusage rinfo;
	int runstat;
	int suspend = 0;
	while(true)
	{
		wait4(m_cpid, &runstat, 0, &rinfo);

		m_time_used = rinfo.ru_utime.tv_sec * 1000 + rinfo.ru_utime.tv_usec / 1000;
		++suspend;
		// 问题大大
		if(WIFEXITED(runstat))
		{
			int exitcode = WEXITSTATUS(runstat);

			if(exitcode)
			{
				result(RS_RTE, -exitcode);
				break;
			}
			result(RS_NML, 0);
			break;
		}
		else if(WIFSIGNALED(runstat) ||
			(WIFSTOPPED(runstat) && WSTOPSIG(runstat) != SIGTRAP))
		{
			int signo = 0;
			if(WIFSIGNALED(runstat))
			{
				signo = WTERMSIG(runstat);
				FM_LOG_TRACE("child signaled by %d %s", signo, strsignal(signo));
			}
			else {
				signo = WSTOPSIG(runstat);
				FM_LOG_TRACE("child signaled by %d %s", signo, strsignal(signo));
			}
			//result(RS_ECR, SIGKILL);
			//break;

			switch(signo)
			{
				//TLE
				case SIGALRM:
				case SIGXCPU:
				case SIGVTALRM:
				case SIGKILL:
					FM_LOG_TRACE("Time Limit Exceeded");
					result(RS_TLE, signo);
					break;
				//OLE
				case SIGXFSZ:
					FM_LOG_TRACE("Output Limit Exceeded");
					result(RS_OLE, signo);
					break;
				//RTE
				case SIGSEGV:
				case SIGFPE:
				case SIGBUS:
				case SIGABRT:
					FM_LOG_TRACE("Runtime Error");
					result(RS_RTE, signo);
					break;
				default:
					FM_LOG_TRACE("RE unknow");
					result(RS_RTE, signo);
					break;
			}
		}
		else if(WIFSTOPPED(runstat) && SIGTRAP == WSTOPSIG(runstat))
		{
			int signo = WSTOPSIG(runstat);
			if(signo == SIGTRAP)
			{
				try
				{
					if(witnessSyscall())
					{
						break;
					}
				}
				catch(JudgeException &ex)
				{
					throw;
				}
			}	
		}
		else {
				;
		}

		if(m_time_limit > 0  && m_time_used > m_time_limit)
		{
			FM_LOG_TRACE("Time Limit Exceeded");
			result(RS_TLE, 0);
			break;
		}

		if(ptrace(PTRACE_SYSCALL, m_cpid, NULL, NULL) < 0)
		{
			FM_LOG_WARNING("ptrace syscall fail");
			throw ExecutorException("PTRACE_SYSCALL fail");
		}
	}
}



