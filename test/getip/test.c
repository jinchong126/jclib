#include "net.h"

int main(int argc, char *argv[])
{
	unsigned int ip;
	char ethx[30];

	memset(ethx, 0, sizeof(ethx));
	sprintf(ethx, "%s", "eth0");

	net_get_ip_by_ethx(ethx, &ip);

	printf("%s ip is %s\n", ethx, net_get_ip_by_net(ip));
}
