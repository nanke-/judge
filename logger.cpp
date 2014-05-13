/*************************************************************************
	> File Name: logger.cpp
	> Author: conlan
	> Mail: xiongpingwu@gmail.com 
	> Created Time: 2014年04月26日 星期六 13时01分27秒
 ************************************************************************/

#include "logger.h"

FILE *log_fp 	= NULL;
char *log_filename	= NULL;
int log_opened	 = 0;

char log_extra_info[log_buffer_size];

int log_open(const char *filename)
{
	if(1 == log_opened)
	{
		fprintf(stderr, "logger: log already opened\n");
		return 0;
	}
	int len = strlen(filename);
	log_filename = (char *)malloc(sizeof(char) * len + 1);
	strcpy(log_filename, filename);
	log_fp = fopen(log_filename, "a");
	if(NULL == log_fp)
	{
		perror("cannot open log file");
		exit(1);
	}
	atexit(log_close);
	log_opened = 1;
	log_extra_info[0] = 0;
	FM_LOG_NOTICE("log_open %d", log_opened);
	return 1;
}

void log_close()
{
	if(log_opened)
	{
		FM_LOG_NOTICE("log_close %d", log_opened);
		fclose(log_fp);
		free(log_filename);
		log_fp 	= NULL;
		log_filename = NULL;
		log_opened = 0;
	}
}

// static void log_write(int level, const char *file, 
// 			const int line, const char *fmt, ...)
// {
// 	if(0 == log_opened)
// 	{
// 		fprintf(stderr, "log_open not called yet\n");
// 		exit(1);
// 	}

// 	static char buffer[log_buffer_size];
// 	static char datatime[100];
// 	static char line_str[20];
// 	static time_t now;

// 	now = time(NULL);

// 	strftime(datatime, 99, "%Y-%m-%d %H:%M:%S", localtime(&now));
// 	snprintf(line_str, 19, "%d", line);
// 	va_list ap;
// 	va_start(ap, fmt);
// 	vsnprintf(log_buffer, log_buffer_size, fmt, ap);
// 	va_end(ap);

// 	size_t count = snprintf(buffer, log_buffer_size, 
// 		"--%s--\x7 [%s]\x7 [%s:%d]%s\x7 %s\x7\n",
// 		LOG_LEVEL_NOTE[level], datatime, file, line, log_extra_info, log_buffer);

// 	int log_fd = log_fp->_fileno;

// 	if(0 == flock(log_fd, LOCK_EX))
// 	{
// 		if(write(log_fd, buffer, count) < 0)
// 		{
// 			perror("write error");
// 			exit(1);
// 		}
// 		flock(log_fd, LOCK_UN);
// 	}
// 	else
// 	{
// 		perror("flock error");
// 		exit(1);
// 	}
// }


void log_add_info(const char *info)
{
	int len = strlen(log_extra_info);
	snprintf(log_extra_info + len, log_buffer_size - len, "\x7 [%s]", info);

}

