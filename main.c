#include <stdio.h>

#include <lightx/dispatch.h>
#include <lightx/server.h>

int main(int argc, char *argv)
{
	dispatch_init();
	server_init();
	dispatch_loop();
	return 0;
}















