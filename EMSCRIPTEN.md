# Emscripten

## Build

```
emcmake cmake -B build .
cmake --build build
```

## Link

```
emcc -flto -O3 -fno-exceptions -fno-rtti external/SDL/libSDL3.a external/SDL_image/libSDL3_image.a external/SDL_mixer/libSDL3_mixer.a external/SDL_ttf/libSDL3_ttf.a lib/lua-5.3.5/liblua.a lib/bh_random/libbh_random.a lib/physfs-3.0/libphysfs.a CMakeFiles/breakhack.dir/lib/sqlite3/*.o CMakeFiles/breakhack.dir/src/*.o -o index.html -sASYNCIFY -sASYNCIFY_IGNORE_INDIRECT -sASYNCIFY_ONLY=@../funcs.txt -sENVIRONMENT=web --preload-file assets.pack --preload-file data.pack --closure 1 -sTOTAL_MEMORY=96mb
```
