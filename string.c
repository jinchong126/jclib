/******************************************************************************
	Copyright (C), 2012-2014, technology. Co., Ltd.
	File name:		string.c
	Author:			jinchong
	Version:		1.0
	Date: 			2012/09/20
	Description:	实现字符串操作基本函数
	Others:			
 ******************************************************************************/
/*
* 修改记录
*
* $Log:$
*
*/

#include "base.h"

/******************************************************************************
	Function:		strs_trim
	Description:	去掉输入字符串中全部的换行符以及空格和制表符
	Input:
		s					--待操作的字符串
	Output:			
	Return:			直接返回去掉尾部空格后的结果串
	Others:		 <其它说明>
 ******************************************************************************/
char*	strs_trim(char* s)
{
	long	lLen;
	register int i, k;

	/*	输入为空则直接返回*/
	if(!s) return NULL;

	lLen = (long)strlen(s);
	for(i = 0, k = 0; i < lLen; i++)
	{
		if(' ' != s[i] && 9 != s[i] && 10 != s[i] && 13 != s[i])
			s[k++] = s[i];
	}
	if(k != lLen)
		s[k] = 0;
	return s;
}

/******************************************************************************
	Function:		get_value
	Description:	取字符串中指定字符串开头和指定字符串中间的串
	Input:
		in_buf			--字符串中的起始查找位置
		in_len			--输入串长度
		head_buf		--前间隔字符串
		head_len		--前间隔字符串长度
		tail_buf		--后间隔字符串
		tail_len		--后间隔字符串长度
		out_buf			--取出的串
		out_len			--取出的串的长度
	Output:			
	Return:
		BASE_SUCCESS	--取成功
		BASE_FAILURE	--取失败
	Others:		 <其它说明>
 ******************************************************************************/
int get_value(const unsigned char *in_buf, int in_len,
					 const unsigned char *head_buf, int head_len,
					 const unsigned char *tail_buf, int tail_len,
					 unsigned char *out_buf, int *out_len)
{
	int i, match, head_flag;

	i = 0;
	match = 0;

	while(i < in_len)
	{
		if(in_buf[i] == head_buf[0])
		{
			if(in_len - i < head_len)
			{
				break;
			}

			if(0 == memcmp(&in_buf[i], head_buf, head_len))
			{
				match = 1;
				break;
			}
		}
		i++;
	}

	if(!match)
		return BASE_FAILURE;
	head_flag = i;

	match = 0;
	i += head_len;

	while(i < in_len)
	{
		if(in_buf[i] == tail_buf[0])
		{
			if(in_len - i < tail_len)
			{
				break;
			}

			if(0 == memcmp(&in_buf[i], tail_buf, tail_len))
			{
				match = 1;
				break;
			}
		}
		i++;
	}
	
	if(!match)
		return BASE_FAILURE;

	if(in_len - i < tail_len)
		return BASE_FAILURE;
	*out_len = i - head_flag - head_len;
	memcpy(out_buf, &in_buf[head_flag + head_len], *out_len);

	return BASE_SUCCESS;
}

/******************************************************************************
	Function:		ASC2BCD
	Description:	将ASCII字符串转换成为BCD码格式
	Input:
		slen				--字符串长度
		s					--待转换的字符串
		bcd					--进行转换处理后的处理BCD码结果串
	Output:			
	Return:			
	Others:		 <其它说明>
 ******************************************************************************/
void	ASC2BCD(long slen, const char* s, char* bcd)
{
	register int	i, k = 0;
	register unsigned char	ch;

	if(!s || !slen)
	{
		if(bcd)
			bcd[0] = 0;
		return;
	}
	/*	这里不能计算长度，因为bcd 可能大小不足
		slen = (0 >= slen) ? strlen(s) : slen;*/
	for(i = 0; i < slen; i++)
	{
		ch = s[i] & 240;				/*	240 = 0xF0*/
		ch = ch >> 4;
		if(9 >= ch)						/*	 9 = 0x09*/
			bcd[k] = 48 | ch;			/*	48 = 0x30*/
		else
			bcd[k] = 64 | (ch - 9);		/*	64 = 0x40, 9 = 0x09*/
		k++;

		ch = s[i] & 15;					/*	15 = 0x0F*/
		if(9 >= ch)						/*	 9 = 0x09*/
			bcd[k] = 48 | ch;			/*	48 = 0x30*/
		else
			bcd[k] = 64 | (ch - 9);		/*	64 = 0x40, 9 = 0x09*/
		k++;
	}
	bcd[k] = 0;
}

/******************************************************************************
	Function:		BCD2ASC
	Description:	将BCD码转换成为ASC码
	Input:
		bcd					--BCD码串
		len					--输出的ASCII字符串串长度
		s					--输出的ASCII字符串
	Output:			
	Return:	
		BASE_SUCCESS		--转换成功
		BASE_FAILURE		--转换失败(输入数据长度错误)
	Others:		 <其它说明>
 ******************************************************************************/
long	BCD2ASC(const char* bcd, long* len, char* s)
{

	long	lLen, lRet = BASE_SUCCESS;
	register char	ch, in;
	register long	i, k = 0;

	if(!s || !bcd)
	{
		*len = 0;
		return 0;
	}
	else
		lLen = (long)strlen(bcd);

	if(lLen % 2)
	{
		log_printf(MSG_ERROR, "函数BCD2ASC()中, 输入的BCD码串长度非偶数!");
		lRet = BASE_FAILURE;
	}

	for(i = 0; i < lLen; i = i + 2)
	{
		in = bcd[i];
		if('9' >= in && '0' <= in)
			ch = in & 15;
		else if(('f' >= in && 'a' <= in) || ('F' >= in && 'A' <= in))
			ch = (in & 15) + 9;
		else
		{
			ch = 0;
			if(BASE_SUCCESS == lRet)
			{
				log_printf(MSG_ERROR, "函数BCD2ASC()中, 输入的BCD码串非法!");
				lRet = BASE_FAILURE;
			}
		}

		ch = ch << 4;
		if(i + 1 < lLen)
			in = bcd[i+1];
		else
			in = '0';

		if('0' <= in && '9' >= in)
			ch |= in & 15;				/*	15 = 0x0F*/
		else if(('f' >= in && 'a' <= in) || ('F' >= in && 'A' <= in))
			ch |= (in & 15) + 9;
		else
		{
			ch |= 0;
			if(BASE_SUCCESS == lRet)
			{
				log_printf(MSG_ERROR, "函数BCD2ASC()中, 输入的BCD码串非法!");
				lRet = BASE_FAILURE;
			}
		}
		s[k] = ch;k++;
	}
	s[k] = 0;
	if(len) *len = k;
	return lRet;
}
