#include <device/map.h>

#define PORT_IO_SPACE_MAX 65535  // 定义端口IO空间的最大地址

#define NR_MAP 16  // 定义最大映射数量
static IOMap maps[NR_MAP] = {};  // 静态数组，存储所有IO映射
static int nr_map = 0;  // 当前已注册的映射数量

/* 设备接口 */
// 添加一个端口IO映射
void add_pio_map(char* name, ioaddr_t addr, uint8_t* space, int len, io_callback_t callback) {
  assert(nr_map < NR_MAP);  // 断言未超出最大映射数量
  assert(addr + len <= PORT_IO_SPACE_MAX);  // 断言地址范围有效
  // 初始化新的映射
  maps[nr_map] = (IOMap){ .name = name, .low = addr, .high = addr + len - 1,
    .space = space, .callback = callback };
  // 记录日志
  Log("Add port-io map '%s' at [0x%08x, 0x%08x]", maps[nr_map].name, maps[nr_map].low, maps[nr_map].high);

  nr_map++;  // 增加映射数量
}

// 通用的端口IO读取函数
uint32_t pio_read_common(ioaddr_t addr, int len) {
  assert(addr + len - 1 < PORT_IO_SPACE_MAX);  // 断言读取地址范围有效
  int mapid = find_mapid_by_addr(maps, nr_map, addr);  // 根据地址查找映射ID
  assert(mapid != -1);  // 断言找到了有效的映射
  return map_read(addr, len, &maps[mapid]);  // 从映射中读取数据
}

// 通用的端口IO写入函数
void pio_write_common(ioaddr_t addr, uint32_t data, int len) {
  assert(addr + len - 1 < PORT_IO_SPACE_MAX);  // 断言写入地址范围有效
  int mapid = find_mapid_by_addr(maps, nr_map, addr);  // 根据地址查找映射ID
  assert(mapid != -1);  // 断言找到了有效的映射
  map_write(addr, data, len, &maps[mapid]);  // 向映射写入数据
}

/* CPU接口 */
// 以下函数是特定长度的端口IO读写接口，直接调用通用读写函数
uint32_t pio_read_l(ioaddr_t addr) { return pio_read_common(addr, 4); }
uint32_t pio_read_w(ioaddr_t addr) { return pio_read_common(addr, 2); }
uint32_t pio_read_b(ioaddr_t addr) { return pio_read_common(addr, 1); }
void pio_write_l(ioaddr_t addr, uint32_t data) { pio_write_common(addr, data, 4); }
void pio_write_w(ioaddr_t addr, uint32_t data) { pio_write_common(addr, data, 2); }
void pio_write_b(ioaddr_t addr, uint32_t data) { pio_write_common(addr, data, 1); }
