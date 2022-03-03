/******************************************************************************
	Copyright (C), 2012-2014, technology. Co., Ltd.
	File name:		kernel.c
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

short checksum(unsigned short* buffer, int size)
{
	unsigned long cksum = 0;

	while(size > 1)
	{
		cksum += *buffer++;
		size -= sizeof(unsigned short);
	}

	if(size){
		cksum += *(unsigned char*)buffer;
	}

	cksum = (cksum>>16) + (cksum&0xffff);
	cksum += (cksum>>16);

	return (unsigned short)(~cksum);
}

void display_buf(unsigned char *buf, int len)
{
	int i;
	if(buf == NULL)
		return;
	printk("================display len :%d=====================\n", len);
	for(i = 0; i < len; i++) {
		printk("%02X ", buf[i]);
		if((i+1)%16 == 0)
			printk("\n");
	}
	if((i+1)%16 != 0)
		printk("\n");
	printk("================display end=====================\n");
	return;
}
