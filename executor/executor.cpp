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
#include <sys/user.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/ptrace.h>

#include "executor.h"

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
	struct rlimit rl_time_limit, rl_output_limit;
	//Time limit
	//stack limit
	//memory limit
	if(m_output_limit > 0)
	{
		//Set Output File Size Limit
		rl_output_limit.rlim_max = rl_output_limit.rlim_cur = m_output_limit;
		setrlimit(RLIMIT_FSIZE, &rl_output_limit);
	}
}

void Executor::runProgram(void) const
{
	//Redirect IO
	freopen("stdin", "r", stdin);
	freopen("stdout", "w", stdout);

	//Set ptrace
	ptrace(PTRACE_TRACEME, 0, NULL, NULL);

	execl(m_program, m_program, NULL);
}

void Executor::result(int rs, int option) const
{
	ptrace(PTRACE_KILL, m_cpid, NULL, NULL);
	printf("Result: %d signal: %d time: %d memory: %d\n", rs, option, m_time_used, m_memory_used);
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
	for(int i = 0; i < 6; ++i)
	{
		fscanf(fps, "%d", &memory);
	}
	fclose(fps);

	int pagesize = getpagesize() / 1024;
	
	return memory *= pagesize;
}

void Executor::witnessSyscall(void)
{
	struct user_regs_struct reg;
	int syscall;
	static int executed = 0;
	int forbidden = 0;

	ptrace(PTRACE_GETREGS, m_cpid, NULL, &reg);
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
		int memory_current = getMemoryUsed();
		if(memory_current > m_memory_used)
			m_memory_used = memory_current;
		if(m_memory_limit > 0 && m_memory_used > m_memory_limit)
		{
			result(RS_MLE, 0);
		}
	}

	if(forbidden)
	{
		result(RS_SYS,syscall);
	}
}

int Executor::getCPid(void) const
{
	return m_cpid;
}

int Executor::getTimeLimit(void) const
{
	return m_time_limit;
}

void exe_timer(int signo)
{
	kill(Exec.getCPid(), SIGUSR1);
	alarm(0);
}

void exe_setTimer()
{
	//setitimer()

	if(Exec.getTimeLimit() > 0)
	{
		if(signal(SIGALRM, exe_timer) == SIG_ERR)
		{
			perror("cannot set timer");
			exit(0);
		}
		alarm(Exec.getTimeLimit() / 1000.0);	
	}
}

void Executor::execute(void)
{
	if((m_cpid = fork()) == 0)
	{
		setLimits();
		runProgram();
		exit(0);
	}

	exe_setTimer();

	struct rusage rinfo;
	int runstat;
	int suspend = 0;
	while(true)
	{
		wait4(m_cpid, &runstat, 0, &rinfo);

		m_time_used = rinfo.ru_utime.tv_sec * 1000 + rinfo.ru_utime.tv_usec / 1000;
		++suspend;

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
		else if(WIFSIGNALED(runstat))
		{
			result(RS_ECR, SIGKILL);
			break;
		}
		else if(WIFSTOPPED(runstat))
		{
			int signo = WSTOPSIG(runstat);
			if(signo == SIGTRAP)
			{
				witnessSyscall();
			}
			else if(signo == SIGUSR1)
			{
				//Ignore
			}
			else if(signo == SIGXFSZ)
			{
				result(RS_OLE, 0);
				break;
			}
			else 
			{
				result(RS_RTE, signo);
				break;
			}
		}
		else {
				;
		}

		if(m_time_limit > 0  && m_time_used > m_time_limit)
		{
			result(RS_TLE, 0);
			break;
		}

		ptrace(PTRACE_SYSCALL, m_cpid, NULL, NULL);
	}
}



