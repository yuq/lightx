#ifndef _LIGHTX_XPROTO_H_
#define _LIGHTX_XPROTO_H_

#include <stdint.h>

struct client;

typedef int (*xproto_handler_t)(struct client *, void *);

struct xproto_extension {
	char name[64];
	xproto_handler_t handler;
	int has_opcode;
	int num_event;
	int num_error;
	int opcode;
	int event;
	int error;
	struct xproto_extension *next;
};

void xproto_extension_register(struct xproto_extension *extension);

int xproto_error(struct client *client, uint8_t code, uint32_t resource,
				 uint8_t major, uint16_t minor);

int xproto_handle_client_request(struct client *client, void *msg, int len);

#endif
















