#pragma once

long sys_open(const char* path);
long sys_read(int fd, char *buffer, size_t size);