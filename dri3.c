#include <stdio.h>

#include <lightx/client.h>
#include <lightx/xproto.h>

#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/extensions/dri3proto.h>

static int dri3_ext_handler(struct client *client, void *req)
{
	const xReq *r = req;
	printf("%d: glx null req code=%d len=%d\n", client->fd, r->data, r->length);
	return xproto_error(client, BadRequest, 0, r->reqType, r->data);
}

static struct xproto_extension dri3_extension = {
	.name = DRI3_NAME,
	.handler = dri3_ext_handler,
	.has_opcode = 1,
	.num_event = DRI3NumberEvents,
	.num_error = DRI3NumberErrors,
};

void dri3_extension_init(void)
{
	xproto_extension_register(&dri3_extension);
}










