mkdir bin
set PATH=C:\MinGW\bin;%PATH%

g++.exe ^
-Wall ^
-O3 ^
./src/main.cpp ^
./src/gamestate.cpp ^
./src/gamestate_ai.cpp ^
./src/jnes_interface.cpp ^
./src/tetris_player.cpp ^
./src/coefficient_db.cpp ^
-static-libgcc ^
-static-libstdc++ ^
-lgdi32 ^
-o ./bin/run.exe ^
-s

pause

g++.exe ^
-Wall ^
-O3 ^
./src/main_learning.cpp ^
./src/gamestate.cpp ^
./src/gamestate_ai.cpp ^
./src/jnes_interface.cpp ^
./src/tetris_player.cpp ^
./src/coefficient_db.cpp ^
-static-libgcc ^
-static-libstdc++ ^
-lgdi32 ^
-o ./bin/run_learning.exe ^
-s

pause
