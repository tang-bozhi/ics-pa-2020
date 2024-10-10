#ifndef __FS_H__
#define __FS_H__

#include <common.h>

int fs_open(const char* pathname, int flags, int mode);
size_t fs_read(int fd, void* buf, size_t len);
size_t fs_write(int fd, const void* buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

#ifndef SEEK_SET
enum { SEEK_SET, SEEK_CUR, SEEK_END };
//表示文件指针的移动方式。SEEK_SET 表示从文件的开头开始计算，SEEK_CUR 表示从当前文件指针的位置计算，SEEK_END 表示从文件的末尾计算
#endif

#endif
