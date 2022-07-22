galaxy-libretro
---------------

This repo contains the libretro binding code for the Galaksija emulator.

The actual emulator code is in [this repository](https://github.com/nikp123/Galaksija-Emulator),
albeit it's outdated since many improvements were done here.

Building
--------

You'll need a working C/C++ toolchain, git and make.

```
git clone https://github.com/nikp123/galaxy-libretro.git
make
```


Initial setup
-------------

Copy the resulting ``galaksija_libretro.*`` file to your libretro cores folder.


## The optional part

In case you want to change the system firmware of the Galaksija that you are
emulating, you can do so by swapping the following files.
File location within the libretro system folder
 * ``galaksija/CHRGEN.BIN``
 * ``galaksija/ROM1.BIN``
 * ``galaksija/ROM2.BIN`` (optional, but adds math)

The included firmwares won't load if you decide to include any of these files.

An example of such firmwares can be found here:
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


License
-------

The emulator's own code as with the included firmware is in the Public Domain.

External pieces such as:
 * ``libretro-common`` (MIT)
 * ``tinyfiledialogs`` (MIT)
 * ``libz80`` (GPL2)

are licensed differently, so keep that in mind if you plan on
redistributing/modifying the code.


Missing features
----------------

 - audio
 - cassete drive
 - PAL signal emulation (doubt this will ever be implemented, because it's
 stupidly demanding on ANY system)
 - saving the emulation state (can't figure out how that works under libretro)

