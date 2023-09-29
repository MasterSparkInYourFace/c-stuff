#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <linux/if_link.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#define NETLINK_DATA_SIZE 4096

typedef struct RTNL_LinkRequest_s {
	struct  nlmsghdr  header;
	struct  ifinfomsg rtnl_info;
	uint8_t data[NETLINK_DATA_SIZE];
} RTNL_LinkRequest;

ssize_t peeksocket(int s) {
	uint8_t tmp;
	ssize_t r = recv(s, &tmp, 1, MSG_PEEK | MSG_DONTWAIT);
	if (r < 0)
		return -1;
	return r > 0; // data available
};

int main() {
	RTNL_LinkRequest req;
	memset(req.data, 0, NETLINK_DATA_SIZE);

	int rtnl_socket;
	struct sockaddr_nl addr = {
		.nl_family = AF_NETLINK,
		.nl_pad    = 0,
		.nl_pid    = getpid(),
		.nl_groups = 0
	};

	struct sockaddr_nl netlink_kernel = {
		.nl_family = AF_NETLINK,
		.nl_pad    = 0,
		.nl_pid    = 0,
		.nl_groups = 0
	};

	struct iovec netlink_msg_iov;

	struct msghdr netlink_msg = {
		.msg_name       = &netlink_kernel,
		.msg_namelen    = sizeof(struct sockaddr_nl),
		.msg_iov        = &netlink_msg_iov,
		.msg_iovlen     = 1,
		.msg_control    = NULL,
		.msg_controllen = 0,
		.msg_flags      = 0
	};

	size_t sendmsg_ret;

	size_t  res_size;
	struct  nlmsghdr  *res_hdr;
	struct  ifinfomsg *res_rtnl_info;
	struct  rtattr    *res_rtnl_attr;

	rtnl_socket = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);

	if (rtnl_socket < 0) {
		fprintf(stderr, "rtnetlink socket(): %m\n");
		return 1;
	}

	if (bind(rtnl_socket, (struct sockaddr *) &addr, sizeof(struct sockaddr_nl)) < 0) {
		fprintf(stderr, "rtnetlink bind(): %m\n");
		close(rtnl_socket);
		return 1;
	}

	req.header.nlmsg_len   = NLMSG_LENGTH(sizeof(struct ifinfomsg));
	req.header.nlmsg_type  = RTM_GETLINK;
	req.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
	req.header.nlmsg_seq   = 1;
	req.header.nlmsg_pid   = 0;

	req.rtnl_info.ifi_family = AF_UNSPEC;
	req.rtnl_info.ifi_type   = 0;
	req.rtnl_info.ifi_index  = 0;
	req.rtnl_info.ifi_flags  = 0;
	req.rtnl_info.ifi_change = 0xFFFFFFFF;

	netlink_msg_iov.iov_base = &req.header;
	netlink_msg_iov.iov_len  = req.header.nlmsg_len;

	if ((sendmsg_ret = sendmsg(rtnl_socket, &netlink_msg, 0)) < 0) {
		fprintf(stderr, "rtnetlink sendmsg(): %m\n");
		close(rtnl_socket);
		return 1;
	}

	while (1) {
		if ((res_size = recv(rtnl_socket, req.data, NETLINK_DATA_SIZE, 0)) < 0) {
			fprintf(stderr, "rtnetlink recv(): %m\n");
			close(rtnl_socket);
			return 1;
		}

		printf("rtnetlink: %lu bytes received\n", res_size);

		if (res_size < NETLINK_DATA_SIZE || peeksocket(rtnl_socket))
			break;
	}

	close(rtnl_socket);

	return 0;
}
