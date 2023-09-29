#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/ether.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s <interface>\n", argv[0]);
		return 1;
	}
	struct ether_addr a;
	struct ifreq req;
	int s = socket(AF_INET, SOCK_STREAM, 0);
	memset(req.ifr_name, 0, IFNAMSIZ);
	strncpy(req.ifr_name, argv[1], IFNAMSIZ);
	if (ioctl(s, SIOCGIFHWADDR, &req) < 0) {
		fprintf(stderr, "SIOCGIFHWADDR failed: %m\n");
		return 1;
	}
	close(s);
	memcpy(a.ether_addr_octet, req.ifr_hwaddr.sa_data, 6);
	printf("%s: %s\n", argv[1], ether_ntoa(&a));
	return 0;
}
