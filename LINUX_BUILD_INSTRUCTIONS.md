Apart from basic compile tools (*GCC/Clang, Make*) you'll also need to install **sdl2, sdl2-image, sdl2-ttf, sdl2-mixer** (If on a debian based dist you need to install the *dev* packages).
Optionally you can also install **cppcheck** and **physfs**

Once that is done run the following:
```bash
mkdir _build
cd _build
cmake -DCMAKE_BUILD_TYPE=Debug .. # Build type flag is optional
make
```