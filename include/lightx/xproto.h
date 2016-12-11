#ifndef _LIGHTX_XPROTO_H_
#define _LIGHTX_XPROTO_H_

struct client;

typedef int (*xproto_handler_t)(struct client *, void *);

struct xproto_extension {
	char name[64];
	xproto_handler_t *handlers;
	int num_opcode;
	int num_event;
	int num_error;
	int opcode;
	int event;
	int error;
	struct xproto_extension *next;
};

void xproto_extension_register(struct xproto_extension *extension);

int xproto_handle_client_request(struct client *client, void *msg, int len);

#endif
















