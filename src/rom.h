#ifndef __LIBRETRO_ROM_H
#define __LIBRETRO_ROM_H

// Function to load data from file 'CHRGEN.BIN' into out with 2048 bytes allocated (stored compressed in 569 bytes)
void LoadCHRGENBIN(unsigned char* out);

// Function to load data from file 'ROM1.BIN' into out with 4096 bytes allocated (stored compressed in 4184 bytes)
void LoadROM1BIN(unsigned char* out);

// Function to load data from file 'ROM2.BIN' into out with 4096 bytes allocated (stored compressed in 3976 bytes)
void LoadROM2BIN(unsigned char* out);

#endif

