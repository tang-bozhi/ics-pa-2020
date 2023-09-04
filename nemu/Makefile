NAME = nemu

ISA ?= riscv32
ISAS = $(shell ls src/isa/)
ifeq ($(filter $(ISAS), $(ISA)), ) # ISA must be valid
$(error Invalid ISA. Supported: $(ISAS))
endif

ENGINE ?= interpreter
ENGINES = $(shell ls src/engine/)
ifeq ($(filter $(ENGINES), $(ENGINE)), ) # ENGINE must be valid
$(error Invalid ENGINE. Supported: $(ENGINES))
endif

$(info Building $(ISA)-$(NAME)-$(ENGINE))

INC_DIR += ./include ./src/engine/$(ENGINE)
BUILD_DIR ?= ./build

ifdef SHARE
SO = -so#-fPIC:Position Independent Code"的标志。这意味着生成的代码可以在任何地址运行，这是创建共享库所必需的
SO_CFLAGS = -fPIC #Shared Object Compilation Flags 用于编译共享对象的编译标志
-D_SHARE=1#-D是定义宏的标志。这里，它定义了一个名为_SHARE的宏，并为其赋值1
SO_LDLAGS = -shared -fPIC#LDLAGS:这是"Shared Object Linker Flags"的缩写，意思是用于链接共享对象的链接标志
else
LD_LIBS = -lSDL2 -lreadline -ldl
endif

ifndef SHARE
DIFF ?= kvm
ifneq ($(ISA),x86)
ifeq ($(DIFF),kvm)
DIFF = qemu
$(info KVM is only supported with ISA=x86, use QEMU instead)
endif
endif

ifeq ($(DIFF),qemu)
DIFF_REF_PATH = $(NEMU_HOME)/tools/qemu-diff
DIFF_REF_SO = $(DIFF_REF_PATH)/build/$(ISA)-qemu-so
CFLAGS += -D__DIFF_REF_QEMU__
else ifeq ($(DIFF),kvm)
DIFF_REF_PATH = $(NEMU_HOME)/tools/kvm-diff
DIFF_REF_SO = $(DIFF_REF_PATH)/build/$(ISA)-kvm-so
CFLAGS += -D__DIFF_REF_KVM__
else ifeq ($(DIFF),nemu)
DIFF_REF_PATH = $(NEMU_HOME)
DIFF_REF_SO = $(DIFF_REF_PATH)/build/$(ISA)-nemu-interpreter-so
CFLAGS += -D__DIFF_REF_NEMU__
MKFLAGS = ISA=$(ISA) SHARE=1 ENGINE=interpreter
else
$(error invalid DIFF. Supported: qemu kvm nemu)
endif
endif

OBJ_DIR ?= $(BUILD_DIR)/obj-$(ISA)-$(ENGINE)$(SO)
BINARY ?= $(BUILD_DIR)/$(ISA)-$(NAME)-$(ENGINE)$(SO)

include Makefile.git

.DEFAULT_GOAL = app

# Compilation flags 
CC = gcc#CC是一个常见的Makefile变量，它表示C编译器
LD = gcc#LD是一个常见的Makefile变量，它表示链接器
INCLUDES  = $(addprefix -I, $(INC_DIR))#INCLUDES变量包含了一些-I选项，这些选项指定了编译器在查找头文件时应该搜索的目录
CFLAGS   += -O2 -MMD -Wall -Werror -ggdb3 $(INCLUDES) \
            -D__ENGINE_$(ENGINE)__ \
            -D__ISA__=$(ISA) -D__ISA_$(ISA)__ -D_ISA_H_=\"isa/$(ISA).h\"
#CFLAGS += -O2 -MMD -Wall -Werror -ggdb3 $(INCLUDES) -D__ISA__=$(ISA) -fomit-frame-pointer：
#这行代码向CFLAGS变量添加了一些编译选项。CFLAGS是一个常见的Makefile变量，它表示C编译器的选项。这些选项包括优化级别-O2，生成依赖文件-MMD，开启所有
#警告-Wall，将警告视为错误-Werror，生成用于gdb调试的信息-ggdb3，添加包含目录$(INCLUDES)，定义宏__ISA__的值为$(ISA)，以及省略帧指针-fomit-frame-pointer。

# Files to be compiled
SRCS = $(shell find src/ -name "*.c" | grep -v "isa\|engine")
SRCS += $(shell find src/isa/$(ISA) -name "*.c")
SRCS += $(shell find src/engine/$(ENGINE) -name "*.c")
OBJS = $(SRCS:src/%.c=$(OBJ_DIR)/%.o)

# Compilation patterns
$(OBJ_DIR)/%.o: src/%.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(SO_CFLAGS) -c -o $@ $<


# Depencies
-include $(OBJS:.o=.d)

# Some convenient rules

.PHONY: app run gdb clean run-env $(DIFF_REF_SO)
app: $(BINARY)

override ARGS ?= --log=$(BUILD_DIR)/nemu-log.txt
override ARGS += --diff=$(DIFF_REF_SO)

# Command to execute NEMU
IMG :=
NEMU_EXEC := $(BINARY) $(ARGS) $(IMG)

$(BINARY): $(OBJS)
	$(call git_commit, "compile")
	@echo + LD $@
	@$(LD) -O2 -rdynamic $(SO_LDLAGS) -o $@ $^ $(LD_LIBS)

run-env: $(BINARY) $(DIFF_REF_SO)

run: run-env
	$(call git_commit, "run")
	$(NEMU_EXEC)

gdb: run-env
	$(call git_commit, "gdb")
	gdb -s $(BINARY) --args $(NEMU_EXEC)

$(DIFF_REF_SO):
	$(MAKE) -C $(DIFF_REF_PATH) $(MKFLAGS)

clean:
	-rm -rf $(BUILD_DIR)
	$(MAKE) -C tools/gen-expr clean
	$(MAKE) -C tools/qemu-diff clean
	$(MAKE) -C tools/kvm-diff clean
