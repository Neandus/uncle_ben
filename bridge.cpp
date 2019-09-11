#include <linux/if_bridge.h>
#include <linux/limits.h>
#include <linux/if_ether.h>
#include <net/if.h>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <vector>
#include <iostream>

#include "bridge.h"


#define BR_MAX_ENTRY	2048

typedef struct bridge_mac {
	char ifname[IF_NAMESIZE];
	uint8_t addr[ETH_ALEN];
}bridge_mac_t;

static std::vector<bridge_mac_t> lastest_bridge_info;

static std::vector<bridge_mac_t> bridge_read(const char *bridge);
static int bridge_cmp_vec(const std::vector<bridge_mac_t> &bv1, const std::vector<bridge_mac_t> &bv2);
static int bridge_cmp(const bridge_mac_t &b1, const bridge_mac_t &b2);
static void bridge_display_info(const bridge_mac_t & b, std::FILE *stream);


void bridge_update(const char *bridge)
{
	std::vector<bridge_mac_t> current_bridge_info = bridge_read(bridge);

	int vec_cmp_result = bridge_cmp_vec(lastest_bridge_info, current_bridge_info);

	if(vec_cmp_result)
	{
		lastest_bridge_info = current_bridge_info;
		printf("---------------------------------------------------------------\n");
		for(const bridge_mac_t & b : current_bridge_info)
		{
			bridge_display_info(b, stdout);
		}
		printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	}
}

static std::vector<bridge_mac_t> bridge_read(const char *bridge)
{
	struct __fdb_entry fe[BR_MAX_ENTRY];
	char path[PATH_MAX];

	snprintf(path, PATH_MAX, "/sys/class/net/%s/brforward", bridge);

	std::FILE* fd = std::fopen(path, "r");
    if(!fd) {
        std::perror("File opening failed");
        return {};
    }

	int cnt = fread(fe, sizeof(struct __fdb_entry), BR_MAX_ENTRY, fd);
	std::fclose(fd);

	std::vector<bridge_mac_t> bridge_vec;
	for (int i = 0; i < cnt; i++) {
		bridge_mac_t b;
		std::strncpy(b.ifname, bridge, IF_NAMESIZE);
		memcpy(b.addr, fe[i].mac_addr, ETH_ALEN);
		bridge_vec.push_back(b);
	}

	return bridge_vec;
}

static int bridge_cmp_vec(const std::vector<bridge_mac_t> &bv1, const std::vector<bridge_mac_t> &bv2)
{
	if(bv1.size() != bv2.size())
		return 1;

	std::vector<bridge_mac_t>::const_iterator it_bv1 = bv1.cbegin();
	std::vector<bridge_mac_t>::const_iterator it_bv2 = bv2.cbegin();

	int cmp_result = 0;
	for(; it_bv1 != bv1.cend(); it_bv1++, it_bv2++)
	{
		cmp_result += bridge_cmp(*it_bv1, *it_bv2);
	}
	return cmp_result;
}

static int bridge_cmp(const bridge_mac_t &b1, const bridge_mac_t &b2)
{
	if (strcmp(b1.ifname, b2.ifname))
		return 1;
	return memcmp(b1.addr, b2.addr, ETH_ALEN);
}

static void bridge_display_info(const bridge_mac_t & b, std::FILE *stream)
{
	fprintf(stream, "interface name: %s\n", b.ifname);
	char mac[19];
	snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
     b.addr[0], b.addr[1], b.addr[2], b.addr[3], b.addr[4], b.addr[5]);
	fprintf(stream, "mac address: %s\n", mac);
}