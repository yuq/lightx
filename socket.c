#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <socket.h>

int socket_create(const char *path)
{
	int fd;
	struct sockaddr_un address;
	socklen_t address_length;
 
	assert((fd = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0);

	unlink(path);

	memset(&address, 0, sizeof(struct sockaddr_un));
	address.sun_family = AF_UNIX;
	strncpy(address.sun_path, path, sizeof(address.sun_path) - 1);

	assert(!bind(fd, (struct sockaddr *) &address, sizeof(struct sockaddr_un)));

	assert(!listen(fd, 5));

	return fd;
}

int socket_accept(int fd)
{
	int ret;
	struct sockaddr_un addr;
	socklen_t addr_len;

	assert((ret = accept(fd, (struct sockaddr *) &addr, &addr_len)) >= 0);
	return ret;
}

int socket_read(int fd, struct socket_data *data)
{
	struct iovec iov = {
		.iov_base = data->buffer,
		.iov_len = data->buffer_len,
	};
	union {
		struct cmsghdr cmsghdr;
		char buf[CMSG_SPACE(data->fds_len * sizeof(int))];
	} cmsgbuf;
	struct msghdr msg = {
		.msg_name = NULL,
		.msg_namelen = 0,
		.msg_iov = &iov,
		.msg_iovlen = 1,
		.msg_control = cmsgbuf.buf,
		.msg_controllen = CMSG_SPACE(sizeof(cmsgbuf.buf)),
	};

	int ret = recvmsg(fd, &msg, 0);
	assert(!msg.msg_flags);
	data->buffer_len = ret;
	data->fds_len = 0;

	struct cmsghdr *hdr;
	if (msg.msg_controllen >= sizeof(struct cmsghdr)) {
		for (hdr = CMSG_FIRSTHDR(&msg); hdr; hdr = CMSG_NXTHDR(&msg, hdr)) {
			if (hdr->cmsg_level == SOL_SOCKET && hdr->cmsg_type == SCM_RIGHTS) {
				int nfd = (hdr->cmsg_len - CMSG_LEN(0)) / sizeof (int);
				memcpy(data->fds + data->fds_len, CMSG_DATA(hdr), nfd * sizeof (int));
				data->fds_len += nfd;
			}
		}
	}
	return 0;
}

int socket_write(int fd, struct socket_data *data)
{
	union {
		struct cmsghdr cmsghdr;
		char buf[CMSG_SPACE(data->fds_len * sizeof(int))];
	} cmsgbuf;
	struct iovec iov = {
		.iov_base = data->buffer,
		.iov_len = data->buffer_len,
	};
	struct msghdr msg = {
		.msg_name = NULL,
		.msg_namelen = 0,
		.msg_iov = &iov,
		.msg_iovlen = 1,
		.msg_control = cmsgbuf.buf,
		.msg_controllen = CMSG_LEN(sizeof(cmsgbuf.buf)),
	};
	struct cmsghdr *hdr = CMSG_FIRSTHDR(&msg);

	hdr->cmsg_len = msg.msg_controllen;
	hdr->cmsg_level = SOL_SOCKET;
	hdr->cmsg_type = SCM_RIGHTS;
	memcpy(CMSG_DATA(hdr), data->fds, data->fds_len * sizeof(int));

	assert(sendmsg(fd, &msg, 0) >= 0);
	return 0;
}










