#include <stdio.h>

#include <lightx/client.h>
#include <lightx/xproto.h>

#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/extensions/presentproto.h>

static int present_ext_handler(struct client *client, void *req)
{
	const xReq *r = req;
	printf("%d: present null req code=%d len=%d\n", client->fd, r->data, r->length);
	return xproto_error(client, BadRequest, 0, r->reqType, r->data);
}

static struct xproto_extension present_extension = {
	.name = PRESENT_NAME,
	.handler = present_ext_handler,
	.has_opcode = 1,
	.num_event = PresentNumberEvents,
	.num_error = PresentNumberErrors,
};

void present_extension_init(void)
{
	xproto_extension_register(&present_extension);
}











