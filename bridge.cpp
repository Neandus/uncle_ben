#include <linux/if_bridge.h>
#include <linux/limits.h>
#include <linux/if_ether.h>
#include <net/if.h>


#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <vector>

#include "bridge.h"


#define BR_MAX_ENTRY	2048

typedef struct bridge_mac {
	char ifname[IF_NAMESIZE];
	uint8_t addr[ETH_ALEN];
}bridge_mac_t;

static void bridge_display_info(const bridge_mac_t & b, FILE *stream)
{
	fprintf(stream, "interface name: %s\n", b.ifname);

	char mac[19];
	snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
         b.addr[0], b.addr[1], b.addr[2], b.addr[3], b.addr[4], b.addr[5]);
	fprintf(stream, "mac address: %s\n", mac);
}

void bridge_read(const char *bridge, FILE *stream)
{
	FILE *fd;
	int i, cnt;
	struct __fdb_entry fe[BR_MAX_ENTRY];
	char path[PATH_MAX];

	snprintf(path, PATH_MAX, "/sys/class/net/%s/brforward", bridge);
	fd = fopen(path, "r");
	if (!fd)
		return;

	cnt = fread(fe, sizeof(struct __fdb_entry), BR_MAX_ENTRY, fd);
	fclose(fd);

	std::vector<bridge_mac_t> bridge_vec;
	for (i = 0; i < cnt; i++) {
		bridge_mac_t b;
		strncpy(b.ifname, bridge, IF_NAMESIZE);
		memcpy(b.addr, fe[i].mac_addr, ETH_ALEN);
		bridge_vec.push_back(b);
	}

	fprintf(stream, "---------------------------------------------------------------\n");
	for(const auto & b : bridge_vec)
	{
		bridge_display_info(b, stream);
	}
	fprintf(stream, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}