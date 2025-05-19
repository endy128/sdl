# sdl

Learning to use C++ and SDL3 to render something which might end up like a retro arcade racing game.

`g++ main.cpp -lSDL3 -o main.bin`

```bash
mkdir build
cmake -B build
cmake --build build # or add --clean-first
./build/2dgame.bin
```

`cmake --build build --clean-first && ./build/2dgame.bin`
