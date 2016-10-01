#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <client.h>
#include <dispatch.h>
#include <socket.h>


static void client_read_handler(struct dispatch_data *data)
{
	int fds[10];
	char buffer[4096];
	struct socket_data sd = {
		.buffer = buffer,
		.buffer_len = 4096,
		.fds = fds,
		.fds_len = 10,
	};

	assert(!socket_read(data->fd, &sd));
	printf("bl=%d b=%x %x\n", sd.buffer_len, buffer[0], buffer[1]);

	close(data->fd);
	free(data);
}

static void client_write_handler(struct dispatch_data *data)
{
	
}

static void client_error_handler(struct dispatch_data *data)
{
	
}

static struct dispatch_handlers client_handlers = {
	.read_handler = client_read_handler,
	.write_handler = client_write_handler,
	.error_handler = client_error_handler,
};

void client_create(int fd)
{
	struct dispatch_data *data = malloc(sizeof(*data));
	assert(data);
	data->fd = fd;
	data->handlers = &client_handlers;
	dispatch_add(data);
}
