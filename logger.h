/*************************************************************************
	> File Name: logger.h
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月25日 星期五 18时03分41秒
 ************************************************************************/

#ifndef _LOGGER_H__
#define _LOGGER_H__

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <time.h>
#include <error.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/file.h>

int log_open(const char *filename);
void log_close(void);

static void log_write(int ,const char *, const int, const char *, ...);
void log_add_info(const char *info);

const int LOG_FATAL  	= 0;
const int LOG_WARNING 	= 1;
const int LOG_MONITOR 	= 2;
const int LOG_NOTICE	= 3;
const int LOG_TRACE		= 4;
const int LOG_DEBUG 	= 5;

static char LOG_LEVEL_NOTE[][10] = 
{ "FATAL", "WARNING", "MONITOR", "NOTICE", "TRACE", "DEBUG" };


#define FM_LOG_DEBUG(x...) 		log_write(LOG_DEBUG, __FILE__, __LINE__, ##x)
#define FM_LOG_TRACE(x...) 		log_write(LOG_TRACE, __FILE__, __LINE__, ##x)
#define FM_LOG_NOTICE(x...)	 	log_write(LOG_NOTICE, __FILE__, __LINE__, ##x)
#define FM_LOG_MOITOR(x...) 	log_write(LOG_MONITOR, __FILE__, __LINE__, ##x)
#define FM_LOG_WARNING(x...)	log_write(LOG_WARNING, __FILE__, __LINE__, ##x)
#define FM_LOG_FATAL(x...) 		log_write(LOG_FATAL, __FILE__, __LINE__, ##x)

extern FILE *log_fp;
extern char *log_filename;
extern int log_opened;

#define log_buffer_size 8192
static char log_buffer[log_buffer_size];
extern char log_extra_info[log_buffer_size];


static void log_write(int level, const char *file, 
			const int line, const char *fmt, ...)
{
	
	if(0 == log_opened)
	{
		fprintf(stderr, "log_open not called yet\n");
		exit(1);
	}

	static char buffer[log_buffer_size];
	static char datatime[100];
	static char line_str[20];
	static time_t now;

	now = time(NULL);

	strftime(datatime, 99, "%Y-%m-%d %H:%M:%S", localtime(&now));
	snprintf(line_str, 19, "%d", line);
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(log_buffer, log_buffer_size, fmt, ap);
	va_end(ap);

	size_t count = snprintf(buffer, log_buffer_size, 
		"--%s--\x7 [%s]\x7 [%s:%d]%s\x7 %s\x7\n",
		LOG_LEVEL_NOTE[level], datatime, file, line, log_extra_info, log_buffer);

	int log_fd = log_fp->_fileno;

	if(0 == flock(log_fd, LOCK_EX))
	{
		if(write(log_fd, buffer, count) < 0)
		{
			perror("write error");
			exit(1);
		}
		flock(log_fd, LOCK_UN);
	}
	else
	{
		perror("flock error");
		exit(1);
	}
}

#endif	//_LOGGER_H__