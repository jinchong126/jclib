/******************************************************************************
	Copyright (C), 2012-2014, technology. Co., Ltd.
	File name:		string.c
	Author:			jinchong
	Version:		1.0
	Date: 			2012/09/20
	Description:	ʵ���ַ���������������
	Others:			
 ******************************************************************************/
/*
* �޸ļ�¼
*
* $Log:$
*
*/

#include "base.h"

/******************************************************************************
	Function:		strs_trim
	Description:	ȥ�������ַ�����ȫ���Ļ��з��Լ��ո���Ʊ��
	Input:
		s					--���������ַ���
	Output:			
	Return:			ֱ�ӷ���ȥ��β���ո��Ľ����
	Others:		 <����˵��>
 ******************************************************************************/
char*	strs_trim(char* s)
{
	long	lLen;
	register int i, k;

	/*	����Ϊ����ֱ�ӷ���*/
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
	Description:	ȡ�ַ�����ָ���ַ�����ͷ��ָ���ַ����м�Ĵ�
	Input:
		in_buf			--�ַ����е���ʼ����λ��
		in_len			--���봮����
		head_buf		--ǰ����ַ���
		head_len		--ǰ����ַ�������
		tail_buf		--�����ַ���
		tail_len		--�����ַ�������
		out_buf			--ȡ���Ĵ�
		out_len			--ȡ���Ĵ��ĳ���
	Output:			
	Return:
		BASE_SUCCESS	--ȡ�ɹ�
		BASE_FAILURE	--ȡʧ��
	Others:		 <����˵��>
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
	Description:	��ASCII�ַ���ת����ΪBCD���ʽ
	Input:
		slen				--�ַ�������
		s					--��ת�����ַ���
		bcd					--����ת�������Ĵ���BCD������
	Output:			
	Return:			
	Others:		 <����˵��>
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
	/*	���ﲻ�ܼ��㳤�ȣ���Ϊbcd ���ܴ�С����
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
	Description:	��BCD��ת����ΪASC��
	Input:
		bcd					--BCD�봮
		len					--�����ASCII�ַ���������
		s					--�����ASCII�ַ���
	Output:			
	Return:	
		BASE_SUCCESS		--ת���ɹ�
		BASE_FAILURE		--ת��ʧ��(�������ݳ��ȴ���)
	Others:		 <����˵��>
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
		log_printf(MSG_ERROR, "����BCD2ASC()��, �����BCD�봮���ȷ�ż��!");
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
				log_printf(MSG_ERROR, "����BCD2ASC()��, �����BCD�봮�Ƿ�!");
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
				log_printf(MSG_ERROR, "����BCD2ASC()��, �����BCD�봮�Ƿ�!");
				lRet = BASE_FAILURE;
			}
		}
		s[k] = ch;k++;
	}
	s[k] = 0;
	if(len) *len = k;
	return lRet;
}
