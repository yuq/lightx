#include <stdio.h>
#include <string.h>

#include <lightx/client.h>
#include <lightx/xproto.h>
#include <lightx/misc.h>

#include <X11/X.h>
#include <X11/Xproto.h>
#include <GL/glxproto.h>
#include <GL/gl.h>

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

static int glx_query_server_string(struct client *client, void *req)
{
	const xGLXQueryServerStringReq *r = req;
	const char *ptr;
	size_t n, len;

	switch (r->name) {
	case GLX_VERSION:
		ptr = "1.4";
        break;
	default:
		return xproto_error(client, BadValue, 0, r->reqType, r->glxCode);
	}

	n = strlen(ptr) + 1;
	len = pad_to_int32(n);
	char buf[len + sz_xGLXQueryVersionReply];
	xGLXQueryServerStringReply *reply = (xGLXQueryServerStringReply *)buf;
	reply->type = X_Reply;
	reply->sequenceNumber = client->sequence_number;
	reply->length = len >> 2;
	reply->n = n;
	memcpy(reply + 1, ptr, n);

	return client_write(client, buf, sizeof(buf), NULL, 0);
}

static int glx_get_visual_configs(struct client *client, void *req)
{
	const xGLXGetVisualConfigsReq *r = req;
	uint32_t buf[sz_xGLXGetVisualConfigsReply / sizeof(uint32_t) + (18 + 0) * 2];
	xGLXGetVisualConfigsReply *reply = (xGLXGetVisualConfigsReply *)buf;
	reply->type = X_Reply;
	reply->sequenceNumber = client->sequence_number;
	reply->length = (18 + 0) * 2;
	reply->numVisuals = 2;
	reply->numProps = 18 + 0;

	int p = sz_xGLXGetVisualConfigsReply >> 2;

	buf[p++] = 0x21;
	buf[p++] = TrueColor;
	buf[p++] = GL_TRUE;

	buf[p++] = 8; //redbits
	buf[p++] = 8; //greenbits
	buf[p++] = 8; //bluebits
	buf[p++] = 8; //alphabits
	buf[p++] = 0; //accumredbits
	buf[p++] = 0; //accumgreenbits
	buf[p++] = 0; //accumbluebits
	buf[p++] = 0; //accumalphabits

	buf[p++] = GL_TRUE; //double buffer
	buf[p++] = GL_FALSE; //stereo

	buf[p++] = 32; //rgbbits
	buf[p++] = 24; //depth
	buf[p++] = 8; //stencil
	buf[p++] = 0; //auxbuffer
	buf[p++] = 0; //level

	buf[p++] = 0x41;
	buf[p++] = TrueColor;
	buf[p++] = GL_TRUE;

	buf[p++] = 8; //redbits
	buf[p++] = 8; //greenbits
	buf[p++] = 8; //bluebits
	buf[p++] = 8; //alphabits
	buf[p++] = 0; //accumredbits
	buf[p++] = 0; //accumgreenbits
	buf[p++] = 0; //accumbluebits
	buf[p++] = 0; //accumalphabits

	buf[p++] = GL_TRUE; //double buffer
	buf[p++] = GL_FALSE; //stereo

	buf[p++] = 32; //rgbbits
	buf[p++] = 32; //depth
	buf[p++] = 8; //stencil
	buf[p++] = 0; //auxbuffer
	buf[p++] = 0; //level

	return client_write(client, buf, sizeof(buf), NULL, 0);
}

static int glx_ext_handler(struct client *client, void *req)
{
	const xReq *r = req;

	switch (r->data) {
	case X_GLXQueryVersion:
		return glx_query_version(client, req);
	case X_GLXQueryServerString:
		return glx_query_server_string(client, req);
	case X_GLXGetVisualConfigs:
		return glx_get_visual_configs(client, req);
	}

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


