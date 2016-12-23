#include <stdio.h>

#include <lightx/client.h>
#include <lightx/xproto.h>

#include <X11/X.h>
#include <X11/Xproto.h>
#include <GL/glxproto.h>

static int glx_query_version(struct client *client, void *req)
{
	const xGLXQueryVersionReq *r = req;
	xGLXQueryVersionReply reply = {
		.type = X_Reply,
		.sequenceNumber = client->sequence_number,
		.length = 0,
		.majorVersion = 1,
		.minorVersion = 4,
	};
	return client_write(client, &reply, sz_xGLXQueryVersionReply, NULL, 0);
}

static int glx_ext_handler(struct client *client, void *req)
{
	const xReq *r = req;

	if (r->data == X_GLXQueryVersion)
		return glx_query_version(client, req);

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
