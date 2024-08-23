#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void* buf, size_t offset, size_t len);
size_t ramdisk_write(void* buf, size_t offset, size_t len);

static uintptr_t loader(PCB* pcb, const char* filename) {
  Elf_Ehdr ehdr;//开一个ELF头表
  ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));//读入ramdisk中的ELF头表的数据(实际上程序应该放在硬盘中,但就如文档所说当前用ram来模拟,因此放在了内存中的静态存储区)
  assert(*(uint32_t*)ehdr.e_ident == 0x464c457f);//必须得是0x7F"ELF"    注意这里时
  Elf_Phdr phdr[ehdr.e_phnum];//程序头表[程序头表数量]
  ramdisk_read(phdr, ehdr.e_phoff, sizeof(Elf_Phdr) * ehdr.e_phnum);//读入ramdisk中的程序头表的数据
  for (int i = 0; i < ehdr.e_phnum; i++) {
    if (phdr->p_type == PT_LOAD) {
      ramdisk_read((void*)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);//这里ramdisk_read使用了memcpy,nemu中的store指令应该会对这里的(void*)phdr[i].p_vaddr做储存
      memset((void*)phdr[i].p_vaddr + phdr[i].p_filesz, 0, phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  return ehdr.e_entry;
}

void naive_uload(PCB* pcb, const char* filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

