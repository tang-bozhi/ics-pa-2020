#include <common.h>

#ifdef HAS_IOE

#include <device/map.h>
#include <SDL2/SDL.h>

#define AUDIO_PORT 0x200 // Note that this is not the standard
#define AUDIO_MMIO 0xa1000200
#define STREAM_BUF 0xa0800000
#define STREAM_BUF_MAX_SIZE 65536

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  nr_reg
};

static uint8_t* sbuf = NULL;
static uint32_t* audio_base = NULL;

static uint32_t sbuf_pos = 0;

static inline void audio_play(void* userdata, uint8_t* stream, int len) {
  // 将音频数据写入 SDL 流
  SDL_memset(stream, 0, len); // 清空流
  uint32_t used_cnt = audio_base[reg_count];
  len = len > used_cnt ? used_cnt : len; // 读取的长度不能超过已用缓冲区大小

  uint32_t sbuf_size = audio_base[reg_sbuf_size];
  if ((sbuf_pos + len) > sbuf_size) {
    // 如果长度超过缓冲区结尾，需要分两段处理
    SDL_MixAudio(stream, sbuf + sbuf_pos, sbuf_size - sbuf_pos, SDL_MIX_MAXVOLUME);
    SDL_MixAudio(stream + (sbuf_size - sbuf_pos), sbuf, len - (sbuf_size - sbuf_pos), SDL_MIX_MAXVOLUME);
  }
  else {
    // 正常情况，直接写入
    SDL_MixAudio(stream, sbuf + sbuf_pos, len, SDL_MIX_MAXVOLUME);
  }
  sbuf_pos = (sbuf_pos + len) % sbuf_size; // 更新缓冲区位置
  audio_base[reg_count] -= len; // 更新缓冲区中剩余样本数
}

void sdl_audio_callback(void* userdata, uint8_t* stream, int len) {
  SDL_memset(stream, 0, len);
  uint32_t used_cnt = audio_base[reg_count];
  len = len > used_cnt ? used_cnt : len;

  uint32_t sbuf_size = audio_base[reg_sbuf_size];
  if ((sbuf_pos + len) > sbuf_size) {
    SDL_MixAudio(stream, sbuf + sbuf_pos, sbuf_size - sbuf_pos, SDL_MIX_MAXVOLUME);
    SDL_MixAudio(stream + (sbuf_size - sbuf_pos),
      sbuf + (sbuf_size - sbuf_pos),
      len - (sbuf_size - sbuf_pos),
      SDL_MIX_MAXVOLUME);
  }
  else
    SDL_MixAudio(stream, sbuf + sbuf_pos, len, SDL_MIX_MAXVOLUME);
  sbuf_pos = (sbuf_pos + len) % sbuf_size;
  audio_base[reg_count] -= len;
}

void init_sound() {
  SDL_AudioSpec s = {};
  s.format = AUDIO_S16SYS;  // 假设系统中音频数据的格式总是使用16位有符号数来表示
  s.userdata = NULL;        // 不使用
  s.freq = audio_base[reg_freq];
  s.channels = audio_base[reg_channels];
  s.samples = audio_base[reg_samples];
  s.callback = sdl_audio_callback;
  SDL_InitSubSystem(SDL_INIT_AUDIO);
  SDL_OpenAudio(&s, NULL);
  SDL_PauseAudio(0);       //播放，可以执行音频子系统的回调函数
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  if (audio_base[reg_init] == 1) {
    init_sound();
    audio_base[reg_init] = 0;
  }
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (void*)new_space(space_size);
  add_pio_map("audio", AUDIO_PORT, (void*)audio_base, space_size, audio_io_handler);
  add_mmio_map("audio", AUDIO_MMIO, (void*)audio_base, space_size, audio_io_handler);

  sbuf = (void*)new_space(STREAM_BUF_MAX_SIZE);
  add_mmio_map("audio-sbuf", STREAM_BUF, (void*)sbuf, STREAM_BUF_MAX_SIZE, NULL);
  audio_base[reg_sbuf_size] = STREAM_BUF_MAX_SIZE;    //确定流缓冲区的大小
}
#endif	/* HAS_IOE */
