#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <lightx/xproto.h>
#include <lightx/client.h>
#include <lightx/misc.h>

#include <X11/X.h>
#include <X11/Xproto.h>

int xproto_error(struct client *client, uint8_t code, uint32_t resource,
				 uint8_t major, uint16_t minor)
{
	xError error = {
		.type = X_Error,
		.errorCode = code,
		.sequenceNumber = client->sequence_number,
		.resourceID = resource,
		.minorCode = minor,
		.majorCode = major,
	};

	return client_write(client, &error, sz_xError, NULL, 0);
}

static int xproto_setup_client(struct client *client, const char *msg, int len)
{
	if (len < sizeof(xConnClientPrefix))
		return 0;

	const xConnClientPrefix *pre = (const xConnClientPrefix *)msg;

	printf("major=%d minor=%d authp=%d auths=%d\n",
		   pre->majorVersion, pre->minorVersion,
		   pre->nbytesAuthProto, pre->nbytesAuthString);

	int auth_size = sizeof(xConnClientPrefix) + pad_to_int32(pre->nbytesAuthProto) + 
		pre->nbytesAuthString;
	if (len < auth_size)
		return 0;

	printf("auth=%s\n", msg + sizeof(xConnClientPrefix));

	char buffer[4096];
	int n = 0;

	xConnSetupPrefix *fix = (xConnSetupPrefix *)buffer;
	fix->success = 1;
	fix->lengthReason = 0;
	fix->majorVersion = 11;
	fix->minorVersion = 0;
	n += sizeof(*fix);

	xConnSetup *setup = (xConnSetup *)(buffer + n);
	setup->release = 0x00000001;
	setup->ridBase = 0x04000000;
	setup->ridMask = 0x001fffff;
	setup->motionBufferSize = 0x100;
	setup->nbytesVendor = 8; // lightx\0
	setup->maxRequestSize = 0xffff;
	setup->numRoots = 1;
	setup->numFormats = 2;
	setup->imageByteOrder = 0;
	setup->bitmapBitOrder = 0;
	setup->bitmapScanlineUnit = 32;
	setup->bitmapScanlinePad = 32;
	setup->minKeyCode = 0x08;
	setup->maxKeyCode = 0x00;
	n += sizeof(*setup);

	char *vender = buffer + n;
	vender[0] = 'l';
	vender[1] = 'i';
	vender[2] = 'g';
	vender[3] = 'h';
	vender[4] = 't';
	vender[5] = 'x';
	vender[6] = '\0';
	vender[7] = '\0';
	n += 8;

	xPixmapFormat *pix = (xPixmapFormat *)(buffer + n);
	pix[0].depth = 24;
	pix[0].bitsPerPixel = 32;
	pix[0].scanLinePad = 32;
	pix[1].depth = 32;
	pix[1].bitsPerPixel = 32;
	pix[1].scanLinePad = 32;
	n += sizeof(*pix) * 2;

	xWindowRoot *root = (xWindowRoot *)(buffer + n);
	root->windowId = 0x101;
	root->defaultColormap = 0x01;
	root->whitePixel = 0x00ffffff;
	root->blackPixel = 0x00000000;
	root->currentInputMask = 0x00;
	root->pixWidth = 1024;
	root->pixHeight = 768;
	root->mmWidth = 302;
	root->mmHeight = 222;
	root->minInstalledMaps = 1;
	root->maxInstalledMaps = 1;
	root->rootVisualID = 0x21;
	root->backingStore = 0;
	root->saveUnders = 0;
	root->rootDepth = 24;
	root->nDepths = 2;
	n += sizeof(*root);

	xDepth *depth = (xDepth *)(buffer + n);
	depth->depth = 24;
	depth->nVisuals = 1;
	n += sizeof(*depth);

	xVisualType *visual = (xVisualType *)(buffer + n);
	visual->visualID = 0x21;
	visual->class = TrueColor;
	visual->bitsPerRGB = 8;
	visual->colormapEntries = 256;
	visual->redMask = 0x00ff0000;
	visual->greenMask = 0x0000ff00;
	visual->blueMask = 0x000000ff;
	n += sizeof(*visual);

	depth = (xDepth *)(buffer + n);
	depth->depth = 32;
	depth->nVisuals = 1;
	n += sizeof(*depth);

	visual = (xVisualType *)(buffer + n);
	visual->visualID = 0x41;
	visual->class = TrueColor;
	visual->bitsPerRGB = 8;
	visual->colormapEntries = 256;
	visual->redMask = 0x00ff0000;
	visual->greenMask = 0x0000ff00;
	visual->blueMask = 0x000000ff;
	n += sizeof(*visual);

	fix->length = (n - sizeof(*fix)) / 4;

	assert(!client_write(client, buffer, n, NULL, 0));

	return auth_size;
}

static int xproto_null(struct client *client, void *req)
{
	const xReq *r = req;
	printf("%d: null req type=%d len=%d\n", client->fd, r->reqType, r->length);
	return xproto_error(client, BadRequest, 0, r->reqType, 0);
}

static int xproto_get_property(struct client *client, void *req)
{
	const xGetPropertyReq *r = req;
	xGetPropertyReply reply = {
		.type = X_Reply,
		.sequenceNumber = client->sequence_number,
		.length = 0,
		.propertyType = None,
		.bytesAfter = 0,
		.nItems = 0,
	};
	return client_write(client, &reply, sz_xGetPropertyReply, NULL, 0);
}

static int xproto_create_gc(struct client *client, void *req)
{
	const xCreateGCReq *r = req;
	printf("%d: CreateGC gc=0x%08x drawable=0x%08x\n", client->fd, 
		   (unsigned int)r->gc, (unsigned int)r->drawable);
	return 0;
}

static struct xproto_extension *xproto_extensions = NULL;

static int xproto_query_extension(struct client *client, void *req)
{
	const xQueryExtensionReq *r = req;

	if ((int)r->length * 4 - sz_xQueryExtensionReq != pad_to_int32(r->nbytes))
		return xproto_error(client, BadValue, 0, r->reqType, 0);

	xQueryExtensionReply reply = {
		.type = X_Reply,
		.sequenceNumber = client->sequence_number,
		.length = 0,
		.present = xFalse,
		.major_opcode = 0,
		.first_event = 0,
		.first_error = 0,
	};
	struct xproto_extension *ext;
	for (ext = xproto_extensions; ext; ext = ext->next) {
		if (!strncmp(ext->name, (const char *)(r + 1), r->nbytes)) {
			reply.present = xTrue;
			reply.major_opcode = ext->opcode;
			reply.first_event = ext->event;
			reply.first_error = ext->error;
			break;
		}
	}

	return client_write(client, &reply, sz_xQueryExtensionReply, NULL, 0);
}

static int xproto_create_colormap(struct client *client, void *req)
{
	const xCreateColormapReq *r = req;
	printf("%d: CreateColormap alloc=%d mid=0x%x win=0x%x visual=0x%x\n",
		   client->fd, r->alloc, (unsigned int)r->mid, (unsigned int)r->window, 
		   (unsigned int)r->visual);
	return 0;
}

static int xproto_create_window(struct client *client, void *req)
{
	const xCreateWindowReq *r = req;
	printf("%d: CreatWindow depth=%d wid=0x%x parent=0x%x "
		   "x=%d y=%d width=%d height=%d board=%d visual=0x%x mask=0x%x\n",
		   client->fd, r->depth, (unsigned int)r->wid, (unsigned int)r->parent,
		   r->x, r->y, r->width, r->height, r->borderWidth,
		   (unsigned int)r->visual, (unsigned int)r->mask);
	return 0;
}

static int xproto_change_property(struct client *client, void *req)
{
	const xChangePropertyReq *r = req;
	printf("%d: ChangeProperty\n", client->fd);
	return 0;
}

static int xproto_map_window(struct client *client, void *req)
{
	const xResourceReq *r = req;
	printf("%d: MapWindow 0x%x\n", client->fd, (unsigned int)r->id);
	return 0;
}

static xproto_handler_t xproto_handlers[256] = {
	[0] = xproto_null,
	[X_CreateWindow] = xproto_create_window,
	[2 ... 7] = xproto_null,
	[X_MapWindow] = xproto_map_window,
	[9 ... 17] = xproto_null,
	[X_ChangeProperty] = xproto_change_property,
	[19] = xproto_null,
	[X_GetProperty] = xproto_get_property,
	[21 ... 54] = xproto_null,
	[X_CreateGC] = xproto_create_gc,
	[56 ... 77] = xproto_null,
	[X_CreateColormap] = xproto_create_colormap,
	[79 ... 97] = xproto_null,
	[X_QueryExtension] = xproto_query_extension,
	[99 ... 255] = xproto_null,
};
static int num_opcode = 128;
static int num_event = LASTEvent;
static int num_error = FirstExtensionError;

void xproto_extension_register(struct xproto_extension *extension)
{
	extension->next = xproto_extensions;
	xproto_extensions = extension;

	if (extension->has_opcode) {
		xproto_handlers[num_opcode] = extension->handler;
		extension->opcode = num_opcode++;
	}

	if (extension->num_event) {
		extension->event = num_event;
		num_event += extension->num_event;
	}

	if (extension->num_error) {
		extension->error = num_error;
		num_error += extension->num_error;
	}
}

static int xproto_request_handler(struct client *client, void *msg, int len)
{
	xReq *req = msg;

	if (len < sizeof(*req))
		return 0;

	int size = (int)req->length * 4;
	if (len < size)
		return 0;

	int err = xproto_handlers[req->reqType](client, req);
	if (err < 0)
		return err;

	return size;
}

int xproto_handle_client_request(struct client *client, void *msg, int len)
{
	int ret;

	if (client->auth)
		ret = xproto_request_handler(client, msg, len);
	else {
		ret = xproto_setup_client(client, msg, len);
		if (ret > 0)
			client->auth = 1;
	}

	if (ret > 0)
		client->sequence_number++;

	return ret;
}
