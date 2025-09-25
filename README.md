```bash
git clone https://github.com/Eczbek/sand
cd sand
cmake -B build -S . -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake --build build
./build/sand
```

- `W`/`A`/`S`/`D` to move
- `[`/`]` to cycle
- SPACE to interact
- `%` to quit
