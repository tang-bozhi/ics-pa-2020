#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

size_t ramdisk_read(void* buf, size_t offset, size_t len);
size_t ramdisk_write(void* buf, size_t offset, size_t len);

enum { FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB };//文件描述符

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

/**
 * @param pathname 输入路径
 * @param flags 不使用--可以指示打开文件的模式
 * @param mode 不使用--用于指定文件的权限
 *
 *  为了简化实现, 我们允许所有用户程序都可以对所有已存在的文件进行读写, 这样以后, 我们在实现fs_open()的时候就可以忽略flags和mode了.
 * @return fb--file_table结构体数组的下标
 */
int fs_open(const char* pathname, int flags, int mode) {
  int Finfo_num = sizeof(file_table) / sizeof(Finfo);
  for (int i = 0, i++, i < Finfo_num) {
    if (strcmp(file_table[Finfo_num].name, pathname) == 0) {
      return i;
    }
  }
  // 如果未找到文件，终止程序
  assert(0 && "Pathname do not exit");//由于简易文件系统中每一个文件都是固定的, 不会产生新文件, 因此"fs_open()没有找到pathname所指示的文件"属于异常情况, 你需要使用assertion终止程序运行.
  return -1;  // 这是一个不可能到达的代码
}

size_t fs_read(int fd, void* buf, size_t len) {
  Finfo* file = &file_table[fd];
  if (file->size < len) {
    len = file->size;  // 只能读取到文件的末尾
  }
  ramdisk_read(buf, file->disk_offset, len);
  return len;
}

size_t fs_write(int fd, const void* buf, size_t len) {
  Finfo* file = &file_table[fd];
  if (len > file->size) {
    len = file->size;  // 只能写入文件的大小
  }
  ramdisk_write(buf, file->disk_offset, len);
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  Finfo* file = &file_table[fd];
  size_t new_offset = 0;

  if (whence == SEEK_SET) {//从文件的开头开始计算偏移量，偏移量等于offset
    new_offset = offset;
  }
  else if (whence == SEEK_CUR) {//从当前文件指针位置开始计算偏移量，偏移量等于当前偏移量 + offset
    new_offset = file->open_offset + offset;
  }
  else if (whence == SEEK_END) {//从文件的末尾开始计算偏移量，偏移量等于文件末尾位置 + offset
    new_offset = file->size + offset;
  }

  // 确保新的偏移量在文件大小范围内
  if (new_offset > file->size) {
    new_offset = file->size;  // 限制在文件大小范围
  }
  file->open_offset = new_offset; // 更新当前偏移量
  return new_offset;
}

int fs_close(int fd) {
  // 简易文件系统中直接返回 0，表示总是成功
  return 0;
}
