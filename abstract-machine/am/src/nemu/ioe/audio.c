#include <am.h>
#include <nemu.h>

// 定义音频寄存器地址
#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00) // 音频频率寄存器地址，用于设置和读取音频采样频率
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04) // 音频通道寄存器地址，用于设置和读取音频通道数（如单声道或立体声）
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08) // 音频样本寄存器地址，用于设置和读取音频每帧的样本数
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c) // 音频缓冲区大小寄存器地址，用于设置和读取音频缓冲区的大小
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10) // 音频初始化寄存器地址，用于初始化音频系统
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14) // 音频计数寄存器地址，用于读取当前缓冲区中已写入的样本数

// 定义缓冲区位置指针
static uint32_t sbuf_pos = 0;

// 初始化音频子系统
void __am_audio_init() {
}

// 配置音频设备
void __am_audio_config(AM_AUDIO_CONFIG_T* cfg) {
  int bufsize = inl(AUDIO_SBUF_ADDR);
  (*cfg) = (AM_AUDIO_CONFIG_T){
  .present = true,
  .bufsize = bufsize
  };
}

// 控制音频设备
void __am_audio_ctrl(AM_AUDIO_CTRL_T* ctrl) {
  // 设置频率、通道数和样本数
  outl(AUDIO_FREQ_ADDR, ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR, ctrl->samples);

  // 将init寄存器置1，音频子系统进入初始化状态
  outl(AUDIO_INIT_ADDR, 1);
}

// 获取音频状态
void __am_audio_status(AM_AUDIO_STATUS_T* stat) {
  stat->count = inl(AUDIO_COUNT_ADDR);  // 获取当前音频缓冲区中的样本数
}

// 播放音频
void __am_audio_play(AM_AUDIO_PLAY_T* ctl) {
  // 获取音频数据和缓冲区大小
  uint8_t* audio_data = (ctl->buf).start;
  uint32_t sbuf_size = inl(AUDIO_SBUF_SIZE_ADDR);
  uint32_t len = (ctl->buf).end - (ctl->buf).start;

  // 获取音频缓冲区指针
  uint8_t* ab = (uint8_t*)(uintptr_t)AUDIO_SBUF_ADDR;

  // 将音频数据写入缓冲区
  for (int i = 0; i < len; i++) {
    ab[sbuf_pos] = audio_data[i];
    sbuf_pos = (sbuf_pos + 1) % sbuf_size;  // 环形缓冲区处理
  }

  // 更新缓冲区中的样本数
  outl(AUDIO_COUNT_ADDR, inl(AUDIO_COUNT_ADDR) + len);
}
