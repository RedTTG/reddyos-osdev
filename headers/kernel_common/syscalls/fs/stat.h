#pragma once
#include <stdint.h>

/*
 * File type bits (what kind of file this is)
 */

#define S_IFMT   0xF000  /* bitmask for file type */

#define S_IFREG  0x8000  /* regular file (normal file data) */
#define S_IFDIR  0x4000  /* directory (contains entries) */
#define S_IFCHR  0x2000  /* character device (tty, fb, etc.) */
#define S_IFBLK  0x6000  /* block device (disk-like devices) */
#define S_IFIFO  0x1000  /* FIFO / pipe */
#define S_IFLNK  0xA000  /* symbolic link */

/*
 * Permission bits (user/group/other)
 */

#define S_IRUSR  0x0100  /* user has read permission */
#define S_IWUSR  0x0080  /* user has write permission */
#define S_IXUSR  0x0040  /* user has execute permission */

#define S_IRGRP  0x0020  /* group has read permission */
#define S_IWGRP  0x0010  /* group has write permission */
#define S_IXGRP  0x0008  /* group has execute permission */

#define S_IROTH  0x0004  /* others have read permission */
#define S_IWOTH  0x0002  /* others have write permission */
#define S_IXOTH  0x0001  /* others have execute permission */

/*
 * Helper macros (very useful later)
 */

#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)  /* is regular file */
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)  /* is directory */
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)  /* is char device */
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)  /* is block device */
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)  /* is FIFO */
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)  /* is symlink */

typedef struct stat {
    uint64_t st_dev;     // filesystem/device ID
    uint64_t st_ino;     // inode number (or file ID)

    uint32_t st_mode;    // file type + permissions
    uint32_t st_nlink;   // hard link count

    uint32_t st_uid;     // owner user id
    uint32_t st_gid;     // owner group id

    uint64_t st_rdev;    // device ID (if special file)

    int64_t  st_size;    // file size in bytes
    uint64_t st_blksize; // preferred IO block size
    uint64_t st_blocks;  // allocated blocks

    uint64_t st_atime;   // last access time
    uint64_t st_mtime;   // last modification time
    uint64_t st_ctime;   // last status change time
} stat_t;
