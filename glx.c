#include <stdio.h>

#include <lightx/client.h>
#include <lightx/xproto.h>

#include <X11/X.h>
#include <X11/Xproto.h>
#include <GL/glxproto.h>

static int glx_ext_handler(struct client *client, void *req)
{
	const xReq *r = req;
	printf("%d: glx null req code=%d len=%d\n", client->fd, r->data, r->length);
	return xproto_error(client, BadRequest, 0, r->reqType, r->data);
}

static struct xproto_extension glx_extension = {
	.name = GLX_EXTENSION_NAME,
	.handler = glx_ext_handler,
	.has_opcode = 1,
	.num_event = __GLX_NUMBER_EVENTS,
	.num_error = __GLX_NUMBER_ERRORS,
};

void glx_extension_init(void)
{
	xproto_extension_register(&glx_extension);
}














