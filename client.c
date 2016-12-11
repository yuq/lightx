#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <lightx/client.h>
#include <lightx/dispatch.h>
#include <lightx/xproto.h>

static struct client *client_create_data(int fd)
{
	struct client *client = malloc(sizeof(*client));
	if (!client)
		return NULL;
	client->fd = fd;
	client->auth = 0;
	client->sequence_number = 0;
	client->bend = 0;
	client->fend = 0;
	return client;
}

static int client_read_handler(struct dispatch_data *data)
{
	struct client *client = data->data;

	if (client->bend != CLIENT_MAX_BUFFER_SIZE && client->fend != CLIENT_MAX_FDS_SIZE) {
		struct socket_data sd = {
			.buffer = client->buffer + client->bend,
			.buffer_len = CLIENT_MAX_BUFFER_SIZE - client->bend,
			.fds = client->fds + client->fend,
			.fds_len = CLIENT_MAX_FDS_SIZE - client->fend,
		};
		assert(!socket_read(data->fd, &sd));
		client->bend += sd.buffer_len;
		client->fend += sd.fds_len;
	}

	int i = 0;
	while (1) {
		int len = xproto_handle_client_request(client, client->buffer + i, client->bend - i);
		if (len > 0)
			i += len;
		else if (len == 0)
			break;
		else
			return len;
	}

	if (i && i != client->bend)
		memmove(client->buffer, client->buffer + i, client->bend - i);
	client->bend -= i;
	return 0;
}

static int client_write_handler(struct dispatch_data *data)
{
	return 0;
}

static int client_error_handler(struct dispatch_data *data)
{
	return 0;
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

	struct client *client = client_create_data(fd);
	assert(client);

	data->fd = fd;
	data->data = client;
	data->handlers = &client_handlers;
	dispatch_add(data);
}
















