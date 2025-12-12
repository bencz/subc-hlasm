/*
 * Quick-and-dirty crt0 test program.
 * Nils M Holm, 2014
 * In the public domain
 *
 * The executable program *must* be called "systest" or
 * the execve() tests will fail!
 *
 * The program will delete the files TESTFILE and
 * TESTFILE2 (below) silently!
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define TESTFILE	"___test.___"
#define TESTFILE2	"___test2.___"

int	verbose = 0;

void error(char *s) {
	printf("error: %s\n", s);
}

int main(int argc, char **argv) {
	char	*n, *m, *k;
	int	fd, fd2;
	int	i, j;
	char	buf[256];
	int	p[4];
	char	*a[4], *e[2];

	if (argc > 1) {
		switch (argv[1][0]) {
		case 'q':
			_exit(5);
		case 'a':
			if (argc > 2 && memcmp(argv[2], "0123456789", 10))
				error("argc/argv");
			_exit(0);
		case 'e':
			if ((n = getenv("FOO")) == NULL ||
			    memcmp(n, "BAR",3)
			)
				error("environ");
			_exit(0);
		case 'v':
			verbose = 1;
			break;
		default:
			_exit(1);
		}
	}

	if (verbose) printf("sbrk\n");
	n = sbrk(1024);
	m = sbrk(-1024);
	k = sbrk(0);
	if (n + 1024 != m || k != n)
		error("sbrk()");

	if (verbose) printf("creat\n");
	fd  = creat(TESTFILE, 0644);
	if (fd < 0)
		error("creat() of new file");
	close(fd);
	fd = creat(TESTFILE, 0644);
	if (fd < 0)
		error("creat() of existing file");

	if (verbose) printf("write\n");
	for (i=0; i<256; i++)
		buf[i] = i;
	for (i=0; i<256; i += 16)
		if (write(fd, buf, i) != i)
			error("write()");

	if (verbose) printf("close\n");
	if (close(fd) < 0) error("close()");

	if (verbose) printf("open\n");
	fd = open(TESTFILE, O_RDONLY);
	if (fd < 0)
		error("_open in read-only mode");
	if (verbose) printf("read\n");
	for (i=0; i<256; i += 16) {
		if (read(fd, buf, i) != i)
			error("read()");
		for (j=0; j<i; j++)
			if (buf[j] != j)
				error("read() read wrong data");
	}
	if (write(fd, "1234567890", 10) >= 0)
		error("write() could write read-only file");
	close(fd);

	fd = open(TESTFILE, O_WRONLY);
	if (fd < 0)
		error("_open in write-only mode");
	if (write(fd, "0123456789", 10) != 10)
		error("write() could not write write-only file");
	close(fd);
	fd = open(TESTFILE, O_RDONLY);
	read(fd, buf, 10);
	if (memcmp(buf, "0123456789", 10))
		error("read() from write-only file returned wrong data");
	close(fd);

	fd = open(TESTFILE, O_RDWR);
	if (fd < 0)
		error("_open in read-write mode");
	if (write(fd, "9876543210", 10) != 10)
		error("write() could not write read/write file");
	close(fd);
	fd = open(TESTFILE, O_RDONLY);
	read(fd, buf, 10);
	if (memcmp(buf, "9876543210", 10))
		error("read() from read/write file returned wrong data");
	close(fd);

	fd = open(TESTFILE, O_RDWR);
	if (fd < 0)
		error("open in read-write mode (2)");
	if (verbose) printf("lseek\n");
	if (lseek(fd, 250, SEEK_SET) != 250)
		error("lseek() from beginning of file");
	if (write(fd, "1111111111", 10) != 10)
		error("write() could not write read/write file (2)");
	if (lseek(fd, -10, SEEK_END) != 1910)
		error("lseek() from end of file");
	if (write(fd, "2222222222", 10) != 10)
		error("write() could not write read/write file (3)");
	if (lseek(fd, -1000, SEEK_CUR) != 920)
		error("lseek() relative backward");
	if (write(fd, "3333333333", 10) != 10)
		error("write() could not write read/write file (4)");
	if (lseek(fd, 500, SEEK_CUR) != 1430)
		error("lseek() relative forward");
	if (write(fd, "4444444444", 10) != 10)
		error("write() could not write read/write file (5)");
	close(fd);

	p[0] = 250;
	p[1] = 1910;
	p[2] = 920;
	p[3] = 1430;
	for (i=0; i<4; i++) {
		fd = open(TESTFILE, O_RDONLY);
		if (fd < 0)
			error("open in read-only mode (3)");
		for (j=0; j<p[i]; j += 10)
			read(fd, buf, 10);
		if (read(fd, buf, 10) != 10)
			error("read() (2)");
		for (j=0; j<10; j++)
			if (buf[j] != i+1+'0')
				error("read() with lseek()"
					" returned wrong data");
		close(fd);
	}

	if (verbose) printf("rename\n");
	if (rename(TESTFILE, TESTFILE2) < 0)
		error("rename() existing file");
	if (rename(TESTFILE, TESTFILE2) == 0)
		error("rename() non-existing file returned success");
	if ((fd = open(TESTFILE, O_RDONLY)) >= 0) {
		error("rename() left original link");
		close(fd);
	}
	if ((fd = open(TESTFILE2, O_RDONLY)) < 0)
		error("rename() failed to create new link");
	else
		close(fd);

	if (verbose) printf("unlink\n");
	if (unlink(TESTFILE2) < 0)
		error("unlink() existing file");
	if (unlink(TESTFILE2) == 0)
		error("unlink() non-existing file returned success");
	if ((fd = open(TESTFILE2, O_RDONLY)) >= 0) {
		error("unlink() failed to remove link");
		close(fd);
	}

#ifndef __dos
	if (verbose) printf("fork/exit/wait\n");
	for (i=0; i<3; i++) {
		switch (fork()) {
		case -1:
			error("fork()");
			break;
		case 0:	
			exit(i);
		default:
			wait(&j);
			if (j != i<<8)
				error("wait() returned wrong exit code");
		}
	}

	if (verbose) printf("execve\n");
	switch(fork()) {
	case -1:
		error("fork() (2)");
		break;
	case 0:
		a[0] = "./systest";
		a[1] = "q";
		a[2] = NULL;
		execve("./systest", a, NULL);
		error("execve()");
		exit(1);
	case 1:
		wait(&j);
		if (j != 5<<8)
			error("wait() with execve(): wrong exit code");
	}

	if (verbose) printf("argc/argv\n");
	switch(fork()) {
	case -1:
		error("fork() (3)");
		break;
	case 0:
		a[0] = "./systest";
		a[1] = "a";
		a[2] = "0123456789";
		a[3] = NULL;
		execve("./systest", a, NULL);
		error("execve() (2)");
		exit(1);
	case 1:
		wait(&j);
	}

	if (verbose) printf("environ\n");
	switch(fork()) {
	case -1:
		error("fork() (4)");
		break;
	case 0:
		a[0] = "./systest";
		a[1] = "e";
		a[2];
		e[0] = "FOO=BAR";
		e[1] = NULL;
		execve("./systest", a, e);
		error("execve() (3)");
		exit(1);
	case 1:
		wait(&j);
	}
#endif

	if (verbose) printf("time\n");
	printf("_time() returned %d\n", _time());

	if (verbose) printf("exit\n");
	_exit(0);
	error("_exit()");
}
