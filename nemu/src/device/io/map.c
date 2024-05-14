#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>
#include <device/map.h>

// 定义最大 I/O 空间大小为 2MB
#define IO_SPACE_MAX (2 * 1024 * 1024)

// 定义一个对齐到页面边界的 I/O 空间数组
static uint8_t io_space[IO_SPACE_MAX] PG_ALIGN = {};
// 指向当前可用 I/O 空间的指针
static uint8_t* p_space = io_space;

/**
 * 分配新的 I/O 空间，并返回指向新空间开始位置的指针。
 * @param size 需要分配的空间大小。
 * @return 指向新分配空间的指针。
 */
uint8_t* new_space(int size) {
  uint8_t* p = p_space;
  // 页面对齐，将 size 向上取整到页面大小
  size = (size + (PAGE_SIZE - 1)) & ~PAGE_MASK;
  // 更新 p_space 指针
  p_space += size;
  // 确保分配的空间没有超出最大 I/O 空间
  assert(p_space - io_space < IO_SPACE_MAX);
  return p;
}

/**
 * 检查给定地址是否在指定 I/O 映射范围内。
 * @param map 指向 I/O 映射的指针。
 * @param addr 要检查的物理地址。
 */
static inline void check_bound(IOMap* map, paddr_t addr) {
  Assert(map != NULL && addr <= map->high && addr >= map->low,
    "address (0x%08x) is out of bound {%s} [0x%08x, 0x%08x] at pc = " FMT_WORD,
    addr, (map ? map->name : "???"), (map ? map->low : 0), (map ? map->high : 0), cpu.pc);
}

/**
 * 调用 I/O 映射的回调函数。
 * @param c 回调函数指针。
 * @param offset 偏移地址。
 * @param len 读/写的长度。
 * @param is_write 标志是写操作。
 */
static inline void invoke_callback(io_callback_t c, paddr_t offset, int len, bool is_write) {
  if (c != NULL) { c(offset, len, is_write); }
}

/**
 * 从指定 I/O 映射地址读取数据。
 * @param addr 要读取的物理地址。
 * @param len 要读取的数据长度。
 * @param map 指向 I/O 映射的指针。
 * @return 读取的数据。
 */
word_t map_read(paddr_t addr, int len, IOMap* map) {
  assert(len >= 1 && len <= 8); // 确保读取长度在有效范围内
  check_bound(map, addr);       // 检查地址是否在映射范围内
  paddr_t offset = addr - map->low; // 计算偏移量
  invoke_callback(map->callback, offset, len, false); // 调用回调函数准备读取数据

  // 从映射空间读取数据，并根据读取长度进行掩码处理
  word_t data = *(word_t*)(map->space + offset) & (~0Lu >> ((8 - len) << 3));
  return data;
}

/**
 * 向指定 I/O 映射地址写入数据。
 * @param addr 要写入的物理地址。
 * @param data 要写入的数据。
 * @param len 要写入的数据长度。
 * @param map 指向 I/O 映射的指针。
 */
void map_write(paddr_t addr, word_t data, int len, IOMap* map) {
  assert(len >= 1 && len <= 8); // 确保写入长度在有效范围内
  check_bound(map, addr);       // 检查地址是否在映射范围内
  paddr_t offset = addr - map->low; // 计算偏移量

  // 将数据写入映射空间
  memcpy(map->space + offset, &data, len);

  // 调用回调函数
  invoke_callback(map->callback, offset, len, true);
}
