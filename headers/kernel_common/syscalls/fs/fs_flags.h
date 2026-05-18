#pragma once

// access modes
#define O_RDONLY  0
#define O_WRONLY  1
#define O_RDWR    2

// behavior flags
#define O_CREAT   (1 << 3)
#define O_TRUNC   (1 << 4)
#define O_APPEND  (1 << 5)
#define O_EXCL    (1 << 6)

// convenience mask
#define O_ACCMODE  0x3