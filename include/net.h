/******************************************************************************
	Copyright (C), 2010-2011, technology. Co., Ltd.
	File name:		net.h
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
#ifndef __NET_H__
#define __NET_H__

#include "base.h"
#include <arpa/inet.h>
#include <net/if.h>

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif
enum{
	MAC_STRING_LEN = 18
};

enum{
	IPV4_LEN = 16,
	IPV6_LEN = 46
};

char *net_get_ip_by_net(int ip_addr);

int net_get_ip_by_ethx(char *ethx, unsigned int *ip);

#endif

