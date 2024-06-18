#include <device/map.h>

#define NR_MAP 16  // 定义最大映射数量

static IOMap maps[NR_MAP] = {};  // 静态数组，用于存储所有MMIO映射
static int nr_map = 0;  // 当前已注册的映射数量

/* 设备接口 */
// 添加一个MMIO映射
void add_mmio_map(char* name, paddr_t addr, uint8_t* space, int len, io_callback_t callback) {
  assert(nr_map < NR_MAP);  // 断言未超出最大映射数量
  // 创建一个新的映射并初始化
  maps[nr_map] = (IOMap){ .name = name, .low = addr, .high = addr + len - 1,
    .space = space, .callback = callback };
  // 记录日志，表示已添加新的映射
  Log("Add mmio map '%s' at [0x%08x, 0x%08x]", maps[nr_map].name, maps[nr_map].low, maps[nr_map].high);

  nr_map++;  // 增加映射数量
}

/* 总线接口 */
// 根据物理地址获取MMIO映射
IOMap* fetch_mmio_map(paddr_t addr) {
  int mapid = find_mapid_by_addr(maps, nr_map, addr);  // 根据地址查找映射ID
  // 如果未找到有效映射则返回NULL，否则返回映射地址
  return (mapid == -1 ? NULL : &maps[mapid]);
}
