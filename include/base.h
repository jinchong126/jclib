/******************************************************************************
	Copyright (C), 2010-2011, technology. Co., Ltd.
	File name:		base.h
	Author:			jinchong
	Version:		2.0
	Date: 			2012/09/20
	Description:	
	Others:
 ******************************************************************************/
/*
* ÐÞ¸Ä¼ÇÂ¼
*
* $Log:$
*
*/
#ifndef __BASE_H__
#define __BASE_H__

#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <limits.h>
#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "log.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

enum{
	BASE_FAILURE = -1,
	BASE_SUCCESS = 0
};

#endif

