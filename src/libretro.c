#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include <sys/stat.h>

#include "galaxy.h"

#include "libretro.h"

#include "rom.h"

#include <file/file_path.h>

#define VIDEO_WIDTH GALAXY_HORIZONTAL_RESOLUTION
#define VIDEO_HEIGHT GALAXY_VERTICAL_RESOLUTION
#define VIDEO_PIXELS VIDEO_WIDTH * VIDEO_HEIGHT

static uint8_t *frame_buf;
static struct retro_log_callback logging;
char retro_base_directory[4096];
char retro_save_directory[4096];
char retro_game_path[4096];
char bios_path[4096];

static char slash = PATH_DEFAULT_SLASH_C();

galaxy_state galaxy;

struct key {
   unsigned int retrok;
   uint8_t emulator;
};

static const struct key keyMap[] = {
   { 0, 0 }, {RETROK_a, 1}, {RETROK_b, 2}, {RETROK_c, 3}, {RETROK_d, 4},
   {RETROK_e, 5}, {RETROK_f, 6}, {RETROK_g, 7}, {RETROK_h, 8}, {RETROK_i, 9},
   {RETROK_j, 10}, {RETROK_k, 11}, {RETROK_l, 12}, {RETROK_m, 13},
   {RETROK_n, 14}, {RETROK_o, 15}, {RETROK_p, 16}, {RETROK_q, 17},
   {RETROK_r, 18}, {RETROK_s, 19}, {RETROK_t, 20}, {RETROK_u, 21},
   {RETROK_v, 22}, {RETROK_w, 23}, {RETROK_x, 24}, {RETROK_y, 25},
   {RETROK_z, 26}, {RETROK_UP, 27}, {RETROK_DOWN, 28}, {RETROK_LEFT, 29},
   {RETROK_BACKSPACE, 29}, {RETROK_RIGHT, 30}, {RETROK_SPACE, 31},
   {RETROK_0, 32}, {RETROK_1, 33}, {RETROK_2, 34}, {RETROK_3, 35},
   {RETROK_4, 36}, {RETROK_5, 37}, {RETROK_6, 38}, {RETROK_7, 39},
   {RETROK_8, 40}, {RETROK_9, 41},{RETROK_COLON, 42}, {RETROK_QUOTE, 43},
   {RETROK_COMMA, 44}, {RETROK_EQUALS, 45}, {RETROK_PERIOD, 46},
   {RETROK_SLASH, 47}, {RETROK_RETURN, 48}, {RETROK_TAB, 49},
   /**{RETROK_LALT, 50}, **/
   {RETROK_DELETE, 51},
   /**{RETROK_SCROLLOCK, 52},**/
   /**{RETROK_LSHIFT, 53}, {RETROK_RSHIFT, 53} skip these because retroarch is buggy**/ };

static int does_file_exist(const char *filename)
{
   struct stat st;
   int result = stat(filename, &st);
   return result == 0;
}

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
   (void)level;
   va_list va;
   va_start(va, fmt);
   vfprintf(stderr, fmt, va);
   va_end(va);
}

static retro_log_printf_t log_cb = fallback_log;

static retro_environment_t environ_cb;

void retro_init(void)
{
   frame_buf = (uint8_t*)malloc(VIDEO_PIXELS * sizeof(uint32_t));
   const char *dir = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir)
   {
      strncpy(retro_base_directory, dir, sizeof(retro_base_directory));
   }

   if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &dir) && dir)
   {
      strncpy(retro_save_directory, dir, sizeof(retro_save_directory));
   }

   memset(frame_buf, 0x00, VIDEO_PIXELS*sizeof(uint32_t));

   bool ignore_firmware = 0;

   snprintf(bios_path, sizeof(bios_path), "%.4074s%cgalaksija%cCHRGEN.BIN", retro_base_directory, slash, slash);
   if (does_file_exist(bios_path) != 1) {
      ignore_firmware = 1;
      log_cb(RETRO_LOG_INFO, "%s NOT FOUND\n", bios_path);
   }

   snprintf(bios_path, sizeof(bios_path), "%.4076s%cgalaksija%cROM1.BIN", retro_base_directory, slash, slash);
   if (does_file_exist(bios_path) != 1) {
      ignore_firmware = 1;
      log_cb(RETRO_LOG_INFO, "%s NOT FOUND\n", bios_path);
   }

   snprintf(bios_path, sizeof(bios_path), "%.4076s%cgalaksija%cROM2.BIN", retro_base_directory, slash, slash);
   if (does_file_exist(bios_path) != 1) {
      log_cb(RETRO_LOG_INFO, "%s NOT FOUND\n", bios_path);
   }

   snprintf(bios_path, sizeof(bios_path), "%.4072s%cgalaksija%cGAL_PLUS.BIN", retro_base_directory, slash, slash);
   if (does_file_exist(bios_path) != 1) {
      log_cb(RETRO_LOG_INFO, "%s NOT FOUND\n", bios_path);
   }

   snprintf(bios_path, sizeof(bios_path), "%.4084s%cgalaksija%c", retro_base_directory, slash, slash);

   galaxy = (galaxy_state){
      .config = (galaxy_config){
         .default_colors = true,
         .cpu_speed = 0,
         .framerate = 0,
         .firmware_path = bios_path,
         .graphics_mode = GALAXY_GRAPHICS_MODE_RGBX8888,
         .system_state_file = NULL,
         .firmware_ignore = ignore_firmware,
      }
   };

   if(ignore_firmware) {
      LoadCHRGENBIN(galaxy.chargen);
      LoadROM1BIN(&galaxy.memory[GALAXY_ROM1_START]);
      LoadROM2BIN(&galaxy.memory[GALAXY_ROM2_START]);
   }

   galaxy_init(&galaxy);
}

void retro_deinit(void)
{
   free(frame_buf);
   frame_buf = NULL;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "galaksija";
   info->library_version  = "0.1";
   info->need_fullpath    = false;
   info->valid_extensions = ".gal";
}

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   float aspect                = 1.185185f;

   info->timing.fps = 50.0;
   info->timing.sample_rate = 0.0;

   info->geometry.base_width   = VIDEO_WIDTH;
   info->geometry.base_height  = VIDEO_HEIGHT;
   info->geometry.max_width    = VIDEO_WIDTH;
   info->geometry.max_height   = VIDEO_HEIGHT;
   info->geometry.aspect_ratio = aspect;
}

void keyboard_event(bool down, unsigned keycode, uint32_t character, uint16_t key_modifiers)
{
   // handle single key
   int i;
   for(i = 0; i < sizeof(keyMap)/sizeof(struct key); i++) {
      if(keyMap[i].retrok == keycode) {
         galaxy.memory[0x2000+keyMap[i].emulator] = down ? 0xFE : 0xFF;
      }
   }

   // handle modifiers
   galaxy.memory[0x2000+50] = (key_modifiers & RETROKMOD_ALT) ? 0xFE : 0xFF;
   galaxy.memory[0x2000+52] = (key_modifiers & RETROKMOD_SCROLLOCK) ? 0xFE : 0xFF;
   galaxy.memory[0x2000+53] = (key_modifiers & RETROKMOD_SHIFT) ? 0xFE : 0xFF;

   // handle special keys
   if((key_modifiers & RETROKMOD_CTRL) && down) {
      switch(keycode) {
         case RETROK_r: galaxy_reset(&galaxy); break;
         case RETROK_n: galaxy_trigger_nmi(&galaxy); break;
      }
   }
}


void retro_set_environment(retro_environment_t cb)
{
   bool no_rom = true;

   environ_cb = cb;

   if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
      log_cb = logging.log;
   else
      log_cb = fallback_log;

   environ_cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_rom);

   static const struct retro_controller_description ports_default[] =
   {
      { "Keyboard + Mouse",    RETRO_DEVICE_KEYBOARD },
      { "Disconnected",        RETRO_DEVICE_NONE },
      { 0 },
   };

   static const struct retro_controller_info ports[] = {
      { ports_default,  0 },
      { 0 },
   };
   environ_cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);

   struct retro_keyboard_callback callback = { keyboard_event };
   environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &callback);
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

void retro_reset(void)
{
   galaxy_reset(&galaxy);
}

static void update_input(void)
{

}


static void check_variables(void)
{

}

//static void audio_callback(void)
//{
//   for (unsigned i = 0; i < 30000 / 60; i++, phase++)
//   {
//      int16_t val = 0x800 * sinf(2.0f * M_PI * phase * 300.0f / 30000.0f);
//      audio_cb(val, val);
//   }
//
//   phase %= 100;
//}
//
//static void audio_set_state(bool enable)
//{
//   (void)enable;
//}

void retro_run(void)
{
   update_input();

   galaxy_run_frame(&galaxy, frame_buf);

   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      check_variables();

   video_cb(frame_buf, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_WIDTH * sizeof(uint32_t));
}

bool retro_load_game(const struct retro_game_info *info)
{
   struct retro_input_descriptor desc[] = {
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },
      { 0 },
   };

   environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      log_cb(RETRO_LOG_INFO, "XRGB8888 is not supported.\n");
      return false;
   }

   if(info != NULL)
      galaxy_load_state(&galaxy, info->path);

   check_variables();

   return true;
}

void retro_unload_game(void)
{
   galaxy_reset(&galaxy);
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_PAL;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
   return false;
}

size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void *data_, size_t size)
{
   return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
   return false;
}

void *retro_get_memory_data(unsigned id)
{
   (void)id;
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   (void)id;
   return 0;
}

void retro_cheat_reset(void)
{}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}

