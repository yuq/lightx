#include <stdio.h>

#include <lightx/dispatch.h>
#include <lightx/server.h>

#include <lightx/glx.h>
#include <lightx/dri3.h>
#include <lightx/present.h>

int main(int argc, char *argv)
{
	dispatch_init();
	server_init();

	glx_extension_init();
	dri3_extension_init();
	present_extension_init();

	dispatch_loop();
	return 0;
}















