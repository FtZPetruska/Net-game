# NET GAME
## by [ROMAINPC](https://github.com/ROMAINPC) / [Pierre Wendling](https://github.com/FtZPetruska) / Pamphile Saltel

This is the continuation of a group project given for the second year (3rd and 4th semesters) at the University of Bordeaux in the Computer Science course.

## Compilation guide

Make sure you have cloned all the submodules before proceeding.
If you are planning on compiling `net_sdl`, you will need the **development libraries** for: 
- `SDL2`
- `SDL2_image`
- `SDL2_ttf`
- `SDL2_mixer`

On Windows make sure to set your environment variables `SDL2*DIR` properly.

For Unix Makefiles :
```
$ cd /path/to/build/folder
$ cmake /path/to/repo
$ ccmake /path/to/repo

Here you can set all the things you want to enable or disable. By default, only the text version of the game is built.
Once you're done, press 'c' to configure.
If needed, you might have to press 'c' to configure again before generating.
Press 'g' to generate.

$ make [-jX]
```
Currently the executables are in subfolders of `src/`, ordering that for a cleaner build is in the plans.

If you are on Windows, you should open the top CMakeLists with Visual Studio and you can chose your options and which executable to run from there.
