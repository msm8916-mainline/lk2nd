// SPDX-License-Identifier: MIT
#include <fcntl.h>
#include <sys/syscall.h>
#include <unistd.h>

/*
 * Alpine 3.14 enabled faccessat2() in musl, which causes problems with older
 * Docker/libseccomp versions that seem to be still used in Drone Cloud.
 * Workaround this problem by avoiding use of the faccessat2() syscall entirely,
 * this should be good enough for the CI build purpose.
 */

int faccessat(int fd, const char *filename, int amode, int flag)
{
	return syscall(SYS_faccessat, fd, filename, amode, flag);
}

int euidaccess(const char *filename, int amode)
{
	return faccessat(AT_FDCWD, filename, amode, AT_EACCESS);
}

int eaccess(const char *filename, int amode)
{
	return euidaccess(filename, amode);
}
