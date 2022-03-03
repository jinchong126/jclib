/******************************************************************************
	Copyright (C), 2012-2014, technology. Co., Ltd.
	File name:		log.c
	Author:			jinchong
	Version:		1.0
	Date: 			2012/09/20
	Description:	实现日志操作
	Others:			
 ******************************************************************************/
/*
* 修改记录
*
* $Log:$
*
*/

#include "base.h"
#include "syslog.h"

static int log_debug_level = MSG_DEBUG;
static int log_debug_timestamp = 1;

#ifdef BASE_DEBUG_SYSLOG
static int syslog_priority(int level)
{
	switch (level) {
	case MSG_MSGDUMP:
	case MSG_DEBUG:
		return LOG_DEBUG;
	case MSG_INFO:
		return LOG_NOTICE;
	case MSG_WARNING:
		return LOG_WARNING;
	case MSG_ERROR:
		return LOG_ERR;
	}
	return LOG_INFO;
}
#endif /* BASE_DEBUG_SYSLOG */

void debug_print_timestamp(void)
{
	int res;
	struct timeval tv;
	res = gettimeofday(&tv, NULL);

	if (res < 0) {
		fprintf(stderr, "gettimeofday");
		return;
	}

	if (!log_debug_timestamp)
		return;

#ifdef BASE_DEBUG_FILE
	if (out_file) {
		fprintf(out_file, "%ld.%06u: ", (long) tv.tv_sec, (unsigned int) tv.tv_usec);
	} else
#endif /*  BASE_DEBUG_FILE */
		printf("%ld.%06u: ", (long) tv.tv_sec, (unsigned int) tv.tv_usec);
}

/******************************************************************************
	Function:		log_printf
	Description:	打印日志
	Input:
		level				--日志级别
		fmt					--待操作的字符串
	Output:			
	Return:
	Others:		 <其它说明>
 ******************************************************************************/
void log_printf(int level, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	if (level >= log_debug_level) {
#ifdef BASE_DEBUG_SYSLOG
		if (log_debug_syslog) {
			vsyslog(syslog_priority(level), fmt, ap);
		} else {
#endif /* BASE_DEBUG_SYSLOG */
		debug_print_timestamp();
#ifdef BASE_DEBUG_FILE
		if (log_out_file) {
			vfprintf(log_out_file, fmt, ap);
			fprintf(log_out_file, "\n");
		} else {
#endif /* BASE_DEBUG_FILE */
		vprintf(fmt, ap);
		printf("\n");
#ifdef BASE_DEBUG_FILE
		}
#endif /* BASE_DEBUG_FILE */
#ifdef BASE_DEBUG_SYSLOG
		}
#endif /* BASE_DEBUG_SYSLOG */
	}
	va_end(ap);
}
