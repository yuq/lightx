#ifndef _LIGHTX_SOCKET_H_
#define _LIGHTX_SOCKET_H_

struct socket_data {
	void *buffer;
	int buffer_len;
	int *fds;
	int fds_len;
};

int socket_create(const char *path);
int socket_accept(int fd);
int socket_read(int fd, struct socket_data *data);
int socket_write(int fd, struct socket_data *data);

#endif




















