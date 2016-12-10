#ifndef _LIGHTX_XPROTO_H_
#define _LIGHTX_XPROTO_H_

struct client;

int xproto_handle_client_request(struct client *client, const char *msg, int len);

#endif
















