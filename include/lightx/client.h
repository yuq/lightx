#ifndef _LIGHTX_CLIENT_H_
#define _LIGHTX_CLIENT_H_

#include <stdint.h>
#include <lightx/socket.h>

#define CLIENT_MAX_BUFFER_SIZE 0x100000
#define CLIENT_MAX_FDS_SIZE 10

struct client {
	int fd;

	int auth;
	uint16_t sequence_number;

	char buffer[CLIENT_MAX_BUFFER_SIZE];
	int bend;

	int fds[CLIENT_MAX_FDS_SIZE];
	int fend;
};

void client_create(int fd);

static inline int client_write(struct client *client, void *buffer, int buffer_len,
							   int *fds, int fds_len)
{
	struct socket_data sd = {
		.buffer = buffer,
		.buffer_len = buffer_len,
		.fds = fds,
		.fds_len = fds_len,
	};

	return socket_write(client->fd, &sd);
}

#endif


















