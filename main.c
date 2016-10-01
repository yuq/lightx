#include <stdio.h>

#include <dispatch.h>
#include <server.h>

int main(int argc, char *argv)
{
	dispatch_init();
	server_init();
	dispatch_loop();
	return 0;
}















