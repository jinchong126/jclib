/******************************************************************************
	Copyright (C), 2012-2014, technology. Co., Ltd.
	File name:		time.c
	Author:			jinchong
	Version:		1.0
	Date: 			2012/09/20
	Description:	实现日期时间操作
	Others:			
 ******************************************************************************/
/*
* 修改记录
*
* $Log:$
*
*/

#include "base.h"
#include "time.h"

/******************************************************************************
	Function:		get_unix_time
	Description:	取系统当前时间，返回系统自1970年来的秒数
	Input:
		cmd_string	- 待执行的命令
	Output:			无
	Return:
		返回系统自1970年来的秒数
	Others:         <其它说明>
 ******************************************************************************/
unsigned long get_unix_time()
{
	return time(NULL);
}
