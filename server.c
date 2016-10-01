#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include <socket.h>
#include <dispatch.h>
#include <server.h>
#include <client.h>


#define SOCKET_PATH "/tmp/.X11-unix/X1"

static void server_read_handler(struct dispatch_data *data)
{
	int fd = socket_accept(data->fd);
	client_create(fd);
}

static void server_write_handler(struct dispatch_data *data)
{
	
}

static void server_error_handler(struct dispatch_data *data)
{
	
}

static struct dispatch_handlers server_handlers = {
	.read_handler = server_read_handler,
	.write_handler = server_write_handler,
	.error_handler = server_error_handler,
};

static void server_create(int fd)
{
	struct dispatch_data *data = malloc(sizeof(*data));
	assert(data);
	data->fd = fd;
	data->handlers = &server_handlers;
	dispatch_add(data);
}

void server_init(void)
{
	int fd = socket_create(SOCKET_PATH);
	server_create(fd);
}



















