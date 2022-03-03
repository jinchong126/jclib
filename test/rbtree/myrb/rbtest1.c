#include "rbtree_rc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

inline int strcmp_addr(void *aa, void *bb)
{
	struct cc *aa2 = (struct cc*)aa;
	struct cc *bb2 = (struct cc*)bb;
	int ret = 0;

	ret = memcmp((char*)aa2->addr, (char *)bb2->addr, 6);
	printf("p1:%p p2:%p", aa, bb);
	printf("==============addr1:%02x:%02x:%02x:%02x:%02x:%02x", aa2->addr[0], aa2->addr[1], aa2->addr[2], aa2->addr[3], aa2->addr[4], aa2->addr[5]);
	printf(" addr2:%02x:%02x:%02x:%02x:%02x:%02x", bb2->addr[0], bb2->addr[1], bb2->addr[2], bb2->addr[3], bb2->addr[4], bb2->addr[5]);
printf("ret :%d LINE:%d\n", ret, __LINE__);
	return ret;
}

int main(void)
{
	struct cc data, data2;
	struct cc *search = NULL;
	printf("rbtest go ... \n");
	

printf("LINE:%d\n", __LINE__);
	data.addr[0] = 0x0;
	data.addr[1] = 0x11;
	data.addr[2] = 0x22;
	data.addr[3] = 0x33;
	data.addr[4] = 0x44;
	data.addr[5] = 0x55;
	data.i = 12345;
	memcpy(&data2, &data, sizeof(struct cc));
	data2.addr[5] = 0x66;

	rbtree_contarner_init(strcmp_addr);

	/*
	 * 插入测试
	 */
printf("@@@@@@@@@@@@@@@@@@@@@@insert begin\n");
	container_insert(&data, sizeof(struct cc));
	container_insert(&data2, sizeof(struct cc));
printf("@@@@@@@@@@@@@@@@@@@@@@insert end\n");

printf("@@@@@@@@@@@@@@@@@@@@@@select begin\n");
	search = (struct cc*)container_search(&data);
	if(search == NULL) {
		printf("\nsearch is null\n");
	}else{
		printf("\nsearch is ok\n");
		display(search);
	}
printf("@@@@@@@@@@@@@@@@@@@@@@select end\n");

printf("@@@@@@@@@@@@@@@@@@@@@@delete begin\n");
container_delete(&data);
printf("@@@@@@@@@@@@@@@@@@@@@@delete end\n");

printf("@@@@@@@@@@@@@@@@@@@@@@flush begin\n");
	container_flush();
printf("@@@@@@@@@@@@@@@@@@@@@@flush end\n");

	struct cc *search2 = container_search(&data2);
	if(search2 == NULL) {
		printf("\nsearch is null\n");
	}else{
		printf("\nsearch is ok\n");
		display(search2);
	}
	return 0;
}
