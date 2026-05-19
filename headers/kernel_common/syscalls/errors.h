#pragma once

#define EPERM      1   /* operation not permitted */
#define ENOENT     2   /* no such file or directory */
#define ESRCH      3   /* no such process */
#define EINTR      4   /* interrupted system call */
#define EIO        5   /* I/O error */
#define ENXIO      6   /* no such device or address */
#define E2BIG      7   /* argument list too long */
#define ENOEXEC    8   /* executable format error */
#define EBADF      9   /* bad file descriptor */
#define ECHILD     10  /* no child processes */
#define EAGAIN     11  /* try again */
#define ENOMEM     12  /* out of memory */
#define EACCES     13  /* permission denied */
#define EFAULT     14  /* bad address */
#define EBUSY      16  /* resource busy */
#define EEXIST     17  /* file exists */
#define ENODEV     19  /* no such device */
#define ENOTDIR    20  /* not a directory */
#define EISDIR     21  /* is a directory */
#define EINVAL     22  /* invalid argument */
#define ENFILE     23  /* system file table full */
#define EMFILE     24  /* process file table full */
#define ENOSPC     28  /* no space left on device */
#define ESPIPE     29  /* illegal seek */
#define EROFS      30  /* read-only filesystem */
#define ENOSYS     38  /* syscall not implemented */