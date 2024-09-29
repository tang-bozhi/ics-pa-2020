# Makefile for AbstractMachine Kernels and Libraries

### *Get a more readable version of this Makefile* by `make html` (requires python-markdown)
html:
	cat Makefile | sed 's/^\([^#]\)/    \1/g' | markdown_py > Makefile.html
.PHONY: html

## 1. Basic Setup and Checks

### Default to create a bare-metal kernel image
ifeq ($(MAKECMDGOALS),)
  MAKECMDGOALS  = image
  .DEFAULT_GOAL = image
endif

### Override checks when `make clean/clean-all/html`
ifeq ($(findstring $(MAKECMDGOALS),clean|clean-all|html),)

### Print build info message
$(info # Building $(NAME)-$(MAKECMDGOALS) [$(ARCH)])

### Check: environment variable `$AM_HOME` looks sane
ifeq ($(wildcard $(AM_HOME)/am/include/am.h),)#wildcard函数的作用是展开Makefile变量中的通配符:这个 wildcard 调用会检查文件 $(AM_HOME)/am/include/am.h 是否存在。如果该文件存在，wildcard 函数会返回其完整路径；如果不存在，它将返回空字符串 
  $(error $$AM_HOME must be an AbstractMachine repo)
endif

### Check: environment variable `$ARCH` must be in the supported list
ARCHS = $(basename $(notdir $(shell ls $(AM_HOME)/scripts/*.mk)))#basename 从文件名序列中取出各个文件名的前缀部分 | notdir函数用于从文件的完整路径中剥离目录部分，只留下文件名 | shell函数允许Makefile执行外部命令，并捕获该命令的输出
ifeq ($(filter $(ARCHS), $(ARCH)), )#$(filter <pattern...>,<text>):以 <pattern> 模式过滤 <text> 字符串中的单词，保留符合模式 <pattern> 的单词。可以 有多个模式。
  $(error Expected $$ARCH in {$(ARCHS)}, Got "$(ARCH)")
endif#如果$(ARCHS)是x86_64-qemu riscv32-nemu等，而$(ARCH)是riscv32-nemu，那么filter $(ARCHS), $(ARCH)的结果就是riscv32-nemu，表示$(ARCH)的值是有效的

### Extract instruction set architecture (`ISA`) and platform from `$ARCH`. Example: `ARCH=x86_64-qemu -> ISA=x86_64; PLATFORM=qemu`
ARCH_SPLIT = $(subst -, ,$(ARCH))#$(subst <from>,<to>,<text>)<from>:这是你想要在 <text> 中查找并替换的字符串   这是你想要将 <from> 替换成的字符串   <text>：这是包含你想要替换的文本的字符串 
ISA        = $(word 1,$(ARCH_SPLIT))#意义参考上方###后的解释		$(word <n>,<text>):<n>：表示你想要提取的单词的位置，其中第一个单词是 1   
PLATFORM   = $(word 2,$(ARCH_SPLIT))#意义参考上方###后的解释

### Checks end here
endif

## 2. General Compilation Targets

### Create the destination directory (`build/$ARCH`)
WORK_DIR  = $(shell pwd)#print work directory pwd返回当前工作目录的绝对路径 
DST_DIR   = $(WORK_DIR)/build/$(ARCH)#目标路径
$(shell mkdir -p $(DST_DIR))#-p:parent mkdir -p命令的-p选项意味着如果目标目录的上级目录不存在，则会一并创建它们 如果目标目录已经存在，不会报错 

### Compilation targets (image or archive)
IMAGE_REL = build/$(NAME)-$(ARCH)#image relative 相对路径-指定生成的图像文件的位置
IMAGE     = $(abspath $(IMAGE_REL))#abspath:absolute path 将上方的相对路径转成绝对路径
ARCHIVE   = $(WORK_DIR)/build/$(NAME)-$(ARCH).a#这行定义了另一个路径变量ARCHIVE，指向编译过程中生成的归档文件（通常是静态库）

### Files to be linked: object files (`.o`) and libraries (`.a`)
OBJS      = $(addprefix $(DST_DIR)/, $(addsuffix .o, $(basename $(SRCS))))#Add Prefix:添加前缀
LIBS     := $(sort $(LIBS) am klib) # lazy evaluation ("=") causes infinite recursions		sort:(这里是升序)排序
LINKAGE   = $(OBJS) \
  $(addsuffix -$(ARCH).a, $(join \
    $(addsuffix /build/, $(addprefix $(AM_HOME)/, $(LIBS))), \
    $(LIBS) \
))#这行代码定义了一个LINKAGE变量，它构造了一个包含所有目标文件（.o文件）和指定库（.a文件）的完整路径列表，这个列表将用于后续的链接步骤。

## 3. General Compilation Flags

### (Cross) compilers, e.g., mips-linux-gnu-g++
AS        = $(CROSS_COMPILE)gcc#汇编器（Assembler）。虽然这里用gcc（GNU Compiler Collection）来代替传统的as，因为gcc可以在调用汇编器前处理一些预处理指令
CC        = $(CROSS_COMPILE)gcc#C语言编译器。用于将C源代码编译成目标文件（.o文件）
CXX       = $(CROSS_COMPILE)g++#C++语言编译器。用于将C++源代码编译成目标文件。这里使用g++，它是GCC中处理C++的部分
LD        = $(CROSS_COMPILE)ld#链接器（Linker）。用于将多个目标文件或库文件链接成单个可执行文件或库文件
OBJDUMP   = $(CROSS_COMPILE)objdump#objdump是一个命令行程序，用于在类Unix操作系统上显示有关对象文件的各种信息。例如，它可以用作反汇编器，以汇编形式查看可执行文件。
OBJCOPY   = $(CROSS_COMPILE)objcopy#用于复制和转换目标文件格式的工具。可以用于从完整的可执行文件中提取二进制镜像，或者进行格式转换。
READELF   = $(CROSS_COMPILE)readelf#用于显示ELF格式文件（Executable and Linkable Format，可执行与可链接格式）的信息的工具。ELF格式是Linux系统中常用的二进制文件格式，用于可执行文件、目标代码、共享库等。

### Compilation flags
INC_PATH += $(WORK_DIR)/include $(addsuffix /include/, $(addprefix $(AM_HOME)/, $(LIBS)))
INCFLAGS += $(addprefix -I, $(INC_PATH))#在C和C++的编译命令中，-I选项后跟一个目录路径，用于指定额外的头文件搜索路径。

CFLAGS   += -O2 -MMD -Wall -Werror -ggdb $(INCFLAGS) \
            -D__ISA__=\"$(ISA)\" -D__ISA_$(shell echo $(ISA) | tr a-z A-Z)__ \
            -D__ARCH__=$(ARCH) -D__ARCH_$(shell echo $(ARCH) | tr a-z A-Z | tr - _) \
            -D__PLATFORM__=$(PLATFORM) -D__PLATFORM_$(shell echo $(PLATFORM) | tr a-z A-Z | tr - _) \
            -DISA_H=\"$(ISA).h\" \
            -DARCH_H=\"arch/$(ARCH).h\" \
            -fno-asynchronous-unwind-tables -fno-builtin -fno-stack-protector \
            -Wno-main
CXXFLAGS +=  $(CFLAGS) -ffreestanding -fno-rtti -fno-exceptions#CXXFLAGS（C++编译器标志）
ASFLAGS  += -MMD $(INCFLAGS)#Assembler Flags 汇编器标志

## 4. Arch-Specific Configurations

### Paste in arch-specific configurations (e.g., from `scripts/x86_64-qemu.mk`)
-include $(AM_HOME)/scripts/$(ARCH).mk

## 5. Compilation Rules

### Rule (compile): a single `.c` -> `.o` (gcc)
$(DST_DIR)/%.o: %.c#$(DST_DIR)/%.o: %.c：此行指定了Make中的一个模式规则。它意味着任何匹配模式$(DST_DIR)/%.o（其中%作为通配符）的文件都依赖于当前目录中同名的.c文件。
	@mkdir -p $(dir $@) && echo + CC $< 
	@$(CC) -std=gnu11 $(CFLAGS) -c -o $@ $(realpath $<)

### Rule (compile): a single `.cc` -> `.o` (g++)
$(DST_DIR)/%.o: %.cc#$< 表示规则中的第一个依赖项	$@ 表示规则的目标文件，也就是规则要生成的文件
	@mkdir -p $(dir $@) && echo + CXX $<
	@$(CXX) -std=c++17 $(CXXFLAGS) -c -o $@ $(realpath $<)

### Rule (compile): a single `.cpp` -> `.o` (g++)
$(DST_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@) && echo + CXX $<
	@$(CXX) -std=c++17 $(CXXFLAGS) -c -o $@ $(realpath $<)

### Rule (compile): a single `.S` -> `.o` (gcc, which calls as)
$(DST_DIR)/%.o: %.S#下一行:-c compile(只编译不连接)	-g debug(用于生成包含调试信息的目标代码)
	@mkdir -p $(dir $@) && echo + AS $<
	@$(AS) $(ASFLAGS) -c -o $@ $(realpath $<)

### Rule (recursive make): build a dependent library (am, klib, ...)
$(LIBS): %:#下一行-C: change directory  -s (或--silent):选项指示 make 在执行时不显示命令本身，只显示命令的输出
	@$(MAKE) -s -C $(AM_HOME)/$* archive
#解释参考相关.md				   #$* 这个变量表示目标模式中 % 及其之前的部分

### Rule (link): objects (`*.o`) and libraries (`*.a`) -> `IMAGE.elf`, the final ELF binary to be packed into image (ld)
$(IMAGE).elf: $(OBJS) am $(LIBS)#ELF（Executable and Linkable Format）:可执行可连接格式
	@echo + LD "->" $(IMAGE_REL).elf
	@$(LD) $(LDFLAGS) -o $(IMAGE).elf $(LINKAGE)
#这条规则的核心作用是通过链接器 ld 将所有的目标文件和库文件合并成一个 ELF 格式的二进制文件 
#$(LINKAGE)是被链接对象

### Rule (archive): objects (`*.o`) -> `ARCHIVE.a` (ar)
#根据教案和GPT推断,这里的ar打包了AM源文件的: 1. gcc将$ISA-nemu的AM实现源文件编译成目标文件, 然后通过ar将这些目标文件作为一个库, 打包成一个归档文件am-$ISA-nemu.a 2. 通过gcc和ar把程序依赖的运行库(如abstract-machine/klib/)也编译并打包成归档文件 
$(ARCHIVE): $(OBJS) 
	@echo + AR "->" $(shell realpath $@ --relative-to .)
	@ar rcs $(ARCHIVE) $(OBJS)
#ar：是用于创建静态库的Unix工具
#rcs：是ar命令的选项，其中r代表替换现有的档案文件中的文件（如果有同名的话），c代表创建档案文件（如果尚不存在的话），而s代表为档案建立索引，加速链接过程。

### Rule (`#include` dependencies): paste in `.d` files generated by gcc on `-MMD`
-include $(addprefix $(DST_DIR)/, $(addsuffix .d, $(basename $(SRCS))))
#处理这些编译器(如gcc)生成的.d依赖文件,使Make能够自动重新编译在头文件改动后依赖这些头文件的源文件

## 6. Miscellaneous

### Build order control
image: image-dep
archive: $(ARCHIVE)
image-dep: $(OBJS) am $(LIBS)
	@echo \# Creating image [$(ARCH)]
.PHONY: image image-dep archive run $(LIBS)#这里将$(LIBS)声明为伪目标是为了强制执行构建命令：通过将$(LIBS)声明为伪目标，可以确保每次执行make命令时，相关的构建命令都会被执行，即使在文件系统中已经存在相应的库文件。

### Clean a single project (remove `build/`)
clean:
	rm -rf Makefile.html $(WORK_DIR)/build/
#-r:Recursion递归   选项-r代表递归删除，用于删除目录及其包含的所有内容
#-f:force强制   选项-f表示强制执行删除操作，忽略不存在的文件，不会因为文件不存在而报错
.PHONY: clean

### Clean all sub-projects within depth 2 (and ignore errors)
CLEAN_ALL = $(dir $(shell find . -mindepth 2 -name Makefile))
clean-all: $(CLEAN_ALL) clean
$(CLEAN_ALL):
	-@$(MAKE) -s -C $@ clean
.PHONY: clean-all $(CLEAN_ALL)
#解释参考相关.md      clean-all目标---abstract machine/makefile.md