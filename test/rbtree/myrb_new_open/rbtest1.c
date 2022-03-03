#include "rbtree.h"
#include "rbtree_com.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct sta_info {
	int i;
	char addr[6];
};

typedef unsigned char u8;

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#define MSG_DEBUG 1
#define MSG_ERROR 1

void display(struct sta_info *xx)
{
	printf("cc-i:%d\n", xx->i);
	printf("cc-i:"MACSTR"\n", MAC2STR(xx->addr));
}

inline int strcmp_addr(void *aa, void *bb)
{
	struct sta_info *aa2 = (struct sta_info*)aa;
	struct sta_info *bb2 = (struct sta_info*)bb;
	int ret = 0;

	ret = memcmp((char*)aa2->addr, (char *)bb2->addr, 6);
	printf("p1:%p p2:%p", aa, bb);
	printf("==============addr1:%02x:%02x:%02x:%02x:%02x:%02x", aa2->addr[0], aa2->addr[1], aa2->addr[2], aa2->addr[3], aa2->addr[4], aa2->addr[5]);
	printf(" addr2:%02x:%02x:%02x:%02x:%02x:%02x", bb2->addr[0], bb2->addr[1], bb2->addr[2], bb2->addr[3], bb2->addr[4], bb2->addr[5]);
printf("ret :%d LINE:%d\n", ret, __LINE__);
	return ret;
}

struct sta_info *ap_get_sta(struct rbtree_container_head *rbh, const u8 *addr)
{
	struct sta_info sta_new, *sta = NULL;
	memcpy(&sta_new.addr, addr, 6);
	wpa_printf(MSG_DEBUG, "xxxxxxxxxxxxx");
	sta = (struct sta_info *)container_search(rbh, &sta_new);
	wpa_printf(MSG_DEBUG, "ap_get_sta success:%p", sta);
	return sta;
}

struct sta_info *ap_sta_add(struct rbtree_container_head *rbh, const u8 *addr)
{
	struct sta_info *sta = NULL;
	struct rbtree_container *sta_node = NULL;

	sta = ap_get_sta(rbh, addr);
	if (sta) {
wpa_printf(MSG_DEBUG, "FILE:%s LINE:%d  ", __FILE__, __LINE__);
		return sta;
	}

	wpa_printf(MSG_DEBUG, "New STA:"MACSTR, MAC2STR(addr));
	sta = NULL;
	sta_node = malloc(sizeof(struct sta_info) + sizeof(struct rbtree_container));
	if (sta_node == NULL) {
		wpa_printf(MSG_ERROR, "malloc sta info failed");
		return NULL;
	}
	sta = (struct sta_info *)(sta_node + sizeof(struct rbtree_container));
	sta_node->rbtree_data = sta;

	/* initialize STA info data */
	memcpy(sta->addr, addr, 6);
	if(container_insert_new(rbh, sta_node) < 0) {
		free(sta);
		wpa_printf(MSG_DEBUG, "container insert sta info error");
		return NULL;
	}
	wpa_printf(MSG_DEBUG, "ap_sta_add success:%p", sta);

	return sta;
}

#if 0
int main(void)
{
	struct rbtree_container_head rbh;
	char addr[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
	struct sta_info data, data2;
	struct sta_info *search = NULL;
	printf("rbtest go ... \n");
	

printf("LINE:%d\n", __LINE__);
	data.addr[0] = 0x0;
	data.addr[1] = 0x11;
	data.addr[2] = 0x22;
	data.addr[3] = 0x33;
	data.addr[4] = 0x44;
	data.addr[5] = 0x55;
	data.i = 12345;
	memcpy(&data2, &data, sizeof(struct sta_info));
	data2.addr[5] = 0x66;

	rbtree_contarner_init(&rbh, strcmp_addr);

	/*
	 * 插入测试
	 */
printf("@@@@@@@@@@@@@@@@@@@@@@insert begin\n");
	ap_sta_add(&rbh, addr);
printf("@@@@@@@@@@@@@@@@@@@@@@insert end\n");

printf("@@@@@@@@@@@@@@@@@@@@@@select begin\n");
	search = (struct sta_info*)ap_get_sta(&rbh, addr);
	if(search == NULL) {
		printf("\nsearch is null\n");
	}else{
		printf("\nsearch is ok\n");
		display(search);
	}
printf("@@@@@@@@@@@@@@@@@@@@@@select end\n");

printf("@@@@@@@@@@@@@@@@@@@@@@delete begin\n");
	return 0;
}

int main(void)
{
	struct rbtree_container_head rbh;
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

	rbtree_contarner_init(&rbh, strcmp_addr);

	/*
	 * 插入测试
	 */
printf("@@@@@@@@@@@@@@@@@@@@@@insert begin\n");
	container_insert(&rbh, &data, sizeof(struct cc));
	container_insert(&rbh, &data2, sizeof(struct cc));
printf("@@@@@@@@@@@@@@@@@@@@@@insert end\n");

printf("@@@@@@@@@@@@@@@@@@@@@@select begin\n");
	search = (struct cc*)container_search(&rbh, &data);
	if(search == NULL) {
		printf("\nsearch is null\n");
	}else{
		printf("\nsearch is ok\n");
		display(search);
	}
printf("@@@@@@@@@@@@@@@@@@@@@@select end\n");

printf("@@@@@@@@@@@@@@@@@@@@@@delete begin\n");
container_delete(&rbh, &data);
printf("@@@@@@@@@@@@@@@@@@@@@@delete end\n");

printf("@@@@@@@@@@@@@@@@@@@@@@flush begin\n");
	container_flush(&rbh);
printf("@@@@@@@@@@@@@@@@@@@@@@flush end\n");

	struct cc *search2 = container_search(&rbh, &data2);
	if(search2 == NULL) {
		printf("\nsearch is null\n");
	}else{
		printf("\nsearch is ok\n");
		display(search2);
	}
	return 0;
}
#endif

int main(void)
{
	struct rbtree_container_head rbh;
	char addr[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
	char addr2[] = {0x02, 0x02, 0x03, 0x04, 0x05, 0x06};
	struct sta_info data, data2;
	struct sta_info *search = NULL;
	printf("rbtest go ... \n");
	

printf("LINE:%d\n", __LINE__);
	data.addr[0] = 0x0;
	data.addr[1] = 0x11;
	data.addr[2] = 0x22;
	data.addr[3] = 0x33;
	data.addr[4] = 0x44;
	data.addr[5] = 0x55;
	data.i = 12345;
	memcpy(&data2, &data, sizeof(struct sta_info));
	data2.addr[5] = 0x66;

	rbtree_contarner_init(&rbh, strcmp_addr);

	/*
	 * 插入测试
	 */
printf("@@@@@@@@@@@@@@@@@@@@@@insert begin\n");
	ap_sta_add(&rbh, addr);
	ap_sta_add(&rbh, addr2);
printf("@@@@@@@@@@@@@@@@@@@@@@insert end\n");

printf("@@@@@@@@@@@@@@@@@@@@@@select begin\n");
	search = (struct sta_info*)ap_get_sta(&rbh, addr);
	if(search == NULL) {
		printf("\nsearch is null\n");
	}else{
		printf("\nsearch is ok\n");
		display(search);
	}
	search = (struct sta_info*)ap_get_sta(&rbh, addr2);
	if(search == NULL) {
		printf("\nsearch is null\n");
	}else{
		printf("\nsearch is ok\n");
		display(search);
	}
printf("@@@@@@@@@@@@@@@@@@@@@@select end\n");

printf("@@@@@@@@@@@@@@@@@@@@@@delete begin\n");
	return 0;
}
