#ifndef _LIGHTX_DISPATCH_H_
#define _LIGHTX_DISPATCH_H_

struct dispatch_data;

struct dispatch_handlers {
	void (*read_handler)(struct dispatch_data *client);
	void (*write_handler)(struct dispatch_data *client);
	void (*error_handler)(struct dispatch_data *client);
};

struct dispatch_data {
	int fd;
	struct dispatch_handlers *handlers;
};

void dispatch_add(struct dispatch_data *data);
void dispatch_loop(void);

#endif




















