galaxy-libretro
---------------

This repo contains the libretro binding code for the Galaksija emulator.

The actual emulator code is in [this repository](https://github.com/nikp123/Galaksija-Emulator).


Building
--------

You'll need a working C/C++ toolchain, git and make.

```
git clone https://github.com/nikp123/galaxy-libretro.git --recursive
make
```


Initial setup
-------------

Copy the resulting ``galaksija_libretro.*`` file to your libretro cores folder.

You will need these files put into your libretro system folder:
```
md5sum                            file location within the libretro system folder
c7ce14d6a81062db862f3ce5afedcfb6  galaksija/ROM1.BIN
c4c4094622eb9f8724908b86c97abc67  galaksija/ROM2.BIN (optional, but adds math)
ecae5c55d75a2fe0871d0ebd95663628  galaksija/CHRGEN.BIN
```

The emulator **WILL NOT** launch without those (two) files.

The files can be obtained here:
[ROM1.BIN](https://github.com/mejs/galaksija/blob/master/roms/ROM%20A/ROM_A_with_ROM_B_init_ver_29.bin?raw=true)
[CHRGEN.BIN](https://raw.githubusercontent.com/mejs/galaksija/master/roms/Character%20Generator%20ROM/CHRGEN_MIPRO.BIN)
[ROM2.BIN](https://github.com/mejs/galaksija/blob/master/roms/ROM%20B/ROM_B.bin?raw=true)


Launching
---------

Launch it via your libretro frontend, and you're off to the races.

Enjoy.

You can launch ".gal" emulator save states.


Built-in shortcuts
------------------

 - ``CTRL+N`` sends a BREAK command (stopping a process)
 - ``CTRL+R`` resets the computer


Missing features
----------------

 - audio
 - cassete drive
 - PAL signal emulation (doubt this will ever be implemented, because it's
 stupidly demanding on ANY system)
 - optimized character drawing (now it just refreshes the entire screen)
 - saving the emulation state (can't figure out how that works under libretro)

