set PATH=C:\MinGW\bin;%PATH%

g++.exe ^
./src/main.cpp ^
./src/gamestate.cpp ^
./src/gamestate_ai.cpp ^
./src/jnes_interface.cpp ^
./src/tetris_player.cpp ^
-static-libgcc ^
-static-libstdc++ ^
-lgdi32 ^
-o ./bin/run.exe

pause