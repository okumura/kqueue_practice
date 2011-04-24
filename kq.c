#include <sys/types.h> /* for kqueue(). */
#include <sys/event.h> /* for kqueue(). */
#include <sys/times.h> /* for kqueue(). */
#include <sys/stat.h>  /* for open(). */
#include <fcntl.h>     /* for open(). */
#include <unistd.h>    /* for close(). */
#include <errno.h>     /* for errno. */
#include <stdio.h>     /* for printf(). */
#include <assert.h>    /* for assert(). */

#define NEVENTS (64)
#define LEN(x) sizeof(x) / sizeof(x[0])
#define EV_ADD_RD(ev, fd, fflags, data, udata) EV_SET(ev, fd, EVFILT_READ, EV_ADD, fflags, data, udata)
#define EV_ADD_WR(ev, fd, fflags, data, udata) EV_SET(ev, fd, EVFILT_WRITE, EV_ADD, fflags, data, udata)

void add_read_read_one_by_one() {
	int kq, fd1, fd2, n;
	struct kevent changes[2], events[NEVENTS];

	kq = kqueue();
	
	fd1 = open("test1.txt", O_RDWR);
	printf("fd1: %d\n", fd1);
	EV_ADD_RD(&changes[0], fd1, 0, 0, NULL);
	EV_ADD_RD(&changes[1], fd1, 0, 0, NULL);
	n = kevent(kq, changes, LEN(changes), events, LEN(events), NULL);
	printf("%s: %d\n", __FUNCTION__, n);
	assert(n == 1);
	
	fd2 = open("test2.txt", O_RDWR);
	printf("fd2: %d\n", fd2);
	EV_ADD_RD(&changes[0], fd2, 0, 0, NULL);
	EV_ADD_RD(&changes[1], fd2, 0, 0, NULL);
	n = kevent(kq, changes, LEN(changes), events, LEN(events), NULL);
	printf("%s: %d\n", __FUNCTION__, n);
	assert(n == 2);
	
	EV_ADD_RD(&changes[0], fd1, 0, 0, NULL);
	EV_ADD_RD(&changes[1], fd2, 0, 0, NULL);
	n = kevent(kq, changes, LEN(changes), events, LEN(events), NULL);
	printf("%s: %d\n", __FUNCTION__, n);
	assert(n == 2);

	n = kevent(kq, changes, LEN(changes), events, LEN(events), NULL);
	printf("%s: %d\n", __FUNCTION__, n);
	assert(n == 2);
	
	close(fd2);
	close(fd1);
	close(kq);
}

void add_read_write_one_by_one() {
	int kq, fd1, fd2, n;
	struct kevent changes[2], events[NEVENTS];

	kq = kqueue();
	
	fd1 = open("test1.txt", O_RDWR);
	printf("fd1: %d\n", fd1);
	EV_ADD_RD(&changes[0], fd1, 0, 0, NULL);
	EV_ADD_WR(&changes[1], fd1, 0, 0, NULL);
	n = kevent(kq, changes, LEN(changes), events, LEN(events), NULL);
	printf("%s: %d\n", __FUNCTION__, n);
	assert(n == 2);
	
	fd2 = open("test2.txt", O_RDWR);
	printf("fd2: %d\n", fd2);
	EV_ADD_RD(&changes[0], fd2, 0, 0, NULL);
	EV_ADD_WR(&changes[1], fd2, 0, 0, NULL);
	n = kevent(kq, changes, LEN(changes), events, LEN(events), NULL);
	printf("%s: %d\n", __FUNCTION__, n);
	assert(n == 4);
	
	n = kevent(kq, changes, LEN(changes), events, LEN(events), NULL);
	printf("%s: %d\n", __FUNCTION__, n);
	assert(n == 4);

	close(fd2);
	n = kevent(kq, NULL, 0, events, LEN(events), NULL);
	printf("%s: %d\n", __FUNCTION__, n);
	assert(n == 2);
	
	close(fd1);
	n = kevent(kq, NULL, 0, NULL, 0, NULL);
	printf("%s: %d\n", __FUNCTION__, n);
	assert(n == 0);
	/*
	we will hang up, if kevent() called.
	n = kevent(kq, NULL, 0, events, LEN(events), NULL);
	printf("%s: %d\n", __FUNCTION__, n);
	assert(n == 0);
	*/
	
	close(kq);
}

int main() {
	add_read_read_one_by_one();
	add_read_write_one_by_one();
	return 0;
}

