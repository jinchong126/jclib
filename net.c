/******************************************************************************
	Copyright (C), 2012-2014, technology. Co., Ltd.
	File name:		net.c
	Author:			jinchong
	Version:		1.0
	Date: 			2012/09/20
	Description:	�����̹��ú���
	Others:			
 ******************************************************************************/
/*
* �޸ļ�¼
*
* $Log:$
*
*/

#include "net.h"

/******************************************************************************
	Function:		net_is_zero_ether_addr
	Description:	�ж�
	Input:
		s					--���������ַ���
	Output:			
	Return:			ֱ�ӷ���ȥ��β���ո��Ľ����
	Others:		 <����˵��>
 ******************************************************************************/
static inline int net_is_zero_ether_addr(const unsigned char *a)
{
	return !(a[0] | a[1] | a[2] | a[3] | a[4] | a[5]);
}

/******************************************************************************
	Function:		net_get_ip_by_net
	Description:	��������IP��ַת��Ϊ���ʮ���Ƹ�ʽ
	Input:
		ip_addr		--BCD�봮
	Output:			��
	Return:
					--IPV4���ʮ���Ƹ�ʽ
	Others:         <����˵��>
 ******************************************************************************/
char *net_get_ip_by_net(int ip_addr)
{
	struct in_addr s;
	static char	buff[IPV4_LEN];

	memset(buff, 0, sizeof(buff));
	s.s_addr = ip_addr;
	inet_ntop(AF_INET, &s.s_addr, buff, INET_ADDRSTRLEN);
	return buff;
}

/******************************************************************************
	Function:		net_ipv6_get_ip_by_net
	Description:	��IPV6��ʽ��ת��Ϊð�Ÿ�ʽ
	Input:
		ip_addr		--��IPV6��ʽ��
	Output:			��
	Return:
					--IPV6ð�Ÿ�ʽ
	Others:         <����˵��>
 ******************************************************************************/
char *net_ipv6_get_ip_by_net(unsigned char *ip_addr)
{
	static char	buff[IPV6_LEN];

	memset(buff, 0, sizeof(buff));

	inet_ntop(AF_INET6, ip_addr, buff, INET6_ADDRSTRLEN);

	return buff;
}

/******************************************************************************
	Function:		net_get_ip_by_dec
	Description:	�����ʮ����ip��ַת��Ϊ������
	Input:
		ip_addr		--���ʮ����IP��ַ
	Output:			��
	Return:
					--������ip��ַ
	Others:         <����˵��>
 ******************************************************************************/
int net_get_ip_by_dec(char *ip_addr)
{
	struct in_addr s;

	inet_pton(AF_INET, ip_addr, (void *)&s);

	return s.s_addr;
}

/******************************************************************************
	Function:		net_get_mac_by_ethx
	Description:	ϵͳ����
	Input:
		ethx		- �ӿ���eth0
	Output:			
		mac		- ʵ��mac��ַ00:0C:29:BD:EE:52
	Return:
		BASE_SUCCESS	- ����ɹ�
		BASE_FAILURE	- ����ʧ��
	Others:         <����˵��>
 ******************************************************************************/
int net_get_mac_by_ethx(char *ethx, char *mac)
{
	int sock;
	struct ifreq ifreq;

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		log_printf(MSG_DEBUG, "socket error errno:%d", errno);
		perror("socket error");
		return BASE_FAILURE;
	}

	strcpy(ifreq.ifr_name, ethx);
	if(ioctl(sock, SIOCGIFHWADDR, &ifreq) < 0)
	{
		log_printf(MSG_DEBUG, "ioctl error errno:%d", errno);
		perror("socket error");
		return BASE_FAILURE;
	}

	memset(mac, 0, MAC_STRING_LEN);
	sprintf(mac, "%02x:%02x:%02x:%02x:%02x:%02x",
			(unsigned char)ifreq.ifr_hwaddr.sa_data[0],
			(unsigned char)ifreq.ifr_hwaddr.sa_data[1],
			(unsigned char)ifreq.ifr_hwaddr.sa_data[2],
			(unsigned char)ifreq.ifr_hwaddr.sa_data[3],
			(unsigned char)ifreq.ifr_hwaddr.sa_data[4],
			(unsigned char)ifreq.ifr_hwaddr.sa_data[5]);
	close(sock);
	return BASE_SUCCESS;
}

/******************************************************************************
	Function:		net_get_ip_by_ethx
	Description:	ϵͳ����
	Input:
		ethx		- �ӿ���eth0
	Output:			
		ip		- 
	Return:
		BASE_SUCCESS	- ����ɹ�
		BASE_FAILURE	- ����ʧ��
	Others:         <����˵��>
 ******************************************************************************/
int net_get_ip_by_ethx(char *ethx, unsigned int *ip)
{
	int sock;
	struct sockaddr_in sin;
	struct ifreq ifreq;

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		log_printf(MSG_DEBUG, "socket error errno:%d", errno);
		perror("socket error");
		return BASE_FAILURE;
	}

	strcpy(ifreq.ifr_name, ethx);
	if(ioctl(sock, SIOCGIFADDR, &ifreq) < 0)
	{
		log_printf(MSG_DEBUG, "ioctl error errno:%d", errno);
		perror("socket error");
		return BASE_FAILURE;
	}

	memcpy(&sin, &ifreq.ifr_addr, sizeof(struct sockaddr_in));
	*ip = sin.sin_addr.s_addr;
	close(sock);
	return BASE_SUCCESS;
}
