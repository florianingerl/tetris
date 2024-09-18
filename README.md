# tetris
We program a tetris game in c++ with opencv

# Compilation
- Install MSYS2 on your windows computer
- Open the MSYS2 MinGW 64 shell
- Install C++ compiler with pacman -S mingw-w64-x86_64-gcc
- Install cmake with pacman -S mingw-w64-x86_64-cmake
- Install make with pacman -S mingw-w64-x86_64-make
- Install opencv with the command pacman -S mingw-w64-x86_64-opencv
- Install qtbase6 with pacman -S mingw-w64-x86_64-qt6-base
- mkdir A
- cd A
- cmake .. -G "MinGW Makefiles"
- mingw32-make
- Launch game.exe

# Playing
Up until now, one random tile is created in the beginning that falls down.
You can move the tile to the left and to the right with the respective arrow keys.
Press Esc to exit the whole game
