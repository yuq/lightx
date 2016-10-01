#include <assert.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <dispatch.h>

static int dispatch_fd;

void dispatch_add(struct dispatch_data *data)
{
	int flags;
	struct epoll_event event;

	assert((flags = fcntl(data->fd, F_GETFL, 0)) >= 0);
	assert(!fcntl(data->fd, F_SETFL, flags | O_NONBLOCK));

	event.events = EPOLLIN;
	event.data.ptr = data;
	assert(!epoll_ctl(dispatch_fd, EPOLL_CTL_ADD, data->fd, &event));
}

void dispatch_loop(void)
{
	while (1) {
		int i, n;
		struct epoll_event events[64];

		assert((n = epoll_wait(dispatch_fd, events, 64, -1)) >= 0);

		for (i = 0; i < n; i++) {
			struct dispatch_data *data = events[i].data.ptr;

			if (events[i].events & EPOLLIN)
				data->handlers->read_handler(data);
			if (events[i].events & EPOLLOUT)
				data->handlers->write_handler(data);
			if (events[i].events & EPOLLERR)
				data->handlers->error_handler(data);
		}
	}
}

int dispatch_init(void)
{
	assert((dispatch_fd = epoll_create1(0)) >= 0);
}










