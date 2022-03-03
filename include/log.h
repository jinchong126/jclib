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
#ifndef __LOG_H__
#define __LOG_H__

#include "time.h"

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

enum { MSG_MSGDUMP, MSG_DEBUG, MSG_INFO, MSG_WARNING, MSG_ERROR };

void log_printf(int level, const char *fmt, ...);

#endif

