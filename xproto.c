#include <assert.h>
#include <stdio.h>

#include <xproto.h>
#include <client.h>
#include <misc.h>

#include <X11/X.h>
#include <X11/Xproto.h>

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

static int xproto_null(struct client *client, const xReq *req)
{
	printf("null req type=%d len=%d\n", req->reqType, req->length);
	return -1;
}

static int xproto_query_extension(struct client *client, const xReq *req)
{
	printf("query extension req type=%d len=%d\n", req->reqType, req->length);
	return -1;
}

static int (*xproto_handlers[256])(struct client *, const xReq *) = {
	[0 ... 97] = xproto_null,
	[98] = xproto_query_extension,
	[99 ... 255] = xproto_null,
};

static int xproto_request_handler(struct client *client, const char *msg, int len)
{
	const xReq *req = (const xReq *)msg;

	if (len < sizeof(*req))
		return 0;

	assert(req->length >= sizeof(*req));

	int size = req->length * 4;
	if (len < size)
		return 0;

	int err = xproto_handlers[req->reqType](client, req);
	if (err < 0)
		return err;

	return size;
}

int xproto_handle_client_request(struct client *client, const char *msg, int len)
{
	int ret;

	if (client->auth)
		ret = xproto_request_handler(client, msg, len);
	else {
		ret = xproto_setup_client(client, msg, len);
		if (ret > 0)
			client->auth = 1;
	}
	return ret;
}
