#include <stdio.h>

#include <lightx/client.h>
#include <lightx/xproto.h>

#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/extensions/presentproto.h>

static int present_query_version(struct client *client, void *req)
{
	const xPresentQueryVersionReq *r = req;
	xPresentQueryVersionReply reply = {
		.type = X_Reply,
		.sequenceNumber = client->sequence_number,
		.length = 0,
		.majorVersion = PRESENT_MAJOR,
		.minorVersion = PRESENT_MINOR,
	};
	return client_write(client, &reply, sz_xPresentQueryVersionReply, NULL, 0);
}

static int present_ext_handler(struct client *client, void *req)
{
	const xReq *r = req;

	switch (r->data) {
	case X_PresentQueryVersion:
		return present_query_version(client, req);
	}

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











