#ifndef __DEVICE_MAP_H__
#define __DEVICE_MAP_H__

#include <monitor/difftest.h>  // 包含用于差异测试的头文件

typedef void(*io_callback_t)(uint32_t, int, bool);  // 定义IO回调函数的类型
uint8_t* new_space(int size);  // 声明分配内存空间的函数

// 定义I/O映射结构体
typedef struct {
  char* name;       // 设备名称
  paddr_t low;      // 映射的起始地址
  paddr_t high;     // 映射的结束地址
  uint8_t* space;   // 指向映射的内存区域
  io_callback_t callback;  // 设备的回调函数，用于读写操作
} IOMap;

// 判断给定地址是否在指定的映射区间内
static inline bool map_inside(IOMap* map, paddr_t addr) {
  return (addr >= map->low && addr <= map->high);
}

// 通过地址查找映射ID，返回映射索引或-1（如果未找到）
static inline int find_mapid_by_addr(IOMap* maps, int size, paddr_t addr) {
  int i;
  for (i = 0; i < size; i++) {
    if (map_inside(maps + i, addr)) {
      difftest_skip_ref();  // 跳过参考测试，以防止DIFF_TEST报错
      return i;
    }
  }
  return -1;
}

// 添加端口I/O映射
void add_pio_map(char* name, ioaddr_t addr, uint8_t* space, int len, io_callback_t callback);
// 添加内存映射I/O
void add_mmio_map(char* name, paddr_t addr, uint8_t* space, int len, io_callback_t callback);

// 从映射区域读取数据
word_t map_read(paddr_t addr, int len, IOMap* map);
// 向映射区域写入数据
void map_write(paddr_t addr, word_t data, int len, IOMap* map);

#endif
