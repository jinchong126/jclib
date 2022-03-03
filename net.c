/******************************************************************************
	Copyright (C), 2012-2014, technology. Co., Ltd.
	File name:		net.c
	Author:			jinchong
	Version:		1.0
	Date: 			2012/09/20
	Description:	网络编程公用函数
	Others:			
 ******************************************************************************/
/*
* 修改记录
*
* $Log:$
*
*/

#include "net.h"

/******************************************************************************
	Function:		net_is_zero_ether_addr
	Description:	判断
	Input:
		s					--待操作的字符串
	Output:			
	Return:			直接返回去掉尾部空格后的结果串
	Others:		 <其它说明>
 ******************************************************************************/
static inline int net_is_zero_ether_addr(const unsigned char *a)
{
	return !(a[0] | a[1] | a[2] | a[3] | a[4] | a[5]);
}

/******************************************************************************
	Function:		net_get_ip_by_net
	Description:	将整数型IP地址转换为点分十进制格式
	Input:
		ip_addr		--BCD码串
	Output:			无
	Return:
					--IPV4点分十进制格式
	Others:         <其它说明>
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
	Description:	将IPV6格式串转换为冒号格式
	Input:
		ip_addr		--将IPV6格式串
	Output:			无
	Return:
					--IPV6冒号格式
	Others:         <其它说明>
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
	Description:	将点分十进制ip地址转换为整数型
	Input:
		ip_addr		--点分十进制IP地址
	Output:			无
	Return:
					--整数型ip地址
	Others:         <其它说明>
 ******************************************************************************/
int net_get_ip_by_dec(char *ip_addr)
{
	struct in_addr s;

	inet_pton(AF_INET, ip_addr, (void *)&s);

	return s.s_addr;
}

/******************************************************************************
	Function:		net_get_mac_by_ethx
	Description:	系统调用
	Input:
		ethx		- 接口名eth0
	Output:			
		mac		- 实际mac地址00:0C:29:BD:EE:52
	Return:
		BASE_SUCCESS	- 处理成功
		BASE_FAILURE	- 处理失败
	Others:         <其它说明>
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
	Description:	系统调用
	Input:
		ethx		- 接口名eth0
	Output:			
		ip		- 
	Return:
		BASE_SUCCESS	- 处理成功
		BASE_FAILURE	- 处理失败
	Others:         <其它说明>
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
