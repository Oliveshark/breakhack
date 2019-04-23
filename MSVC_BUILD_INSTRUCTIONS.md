Compile on Windows with Visual Studio
-------------------------------------

1. Install [Visual Studio Community 2019](https://visualstudio.microsoft.com/vs/community/) with the "Desktop development with C++" workload
2. Install [CMake](https://cmake.org/download/)
3. Install [NSIS](https://nsis.sourceforge.io/Download)
4. Download and unzip [SDL2-devel-2.x.x-VC.zip](https://www.libsdl.org/download-2.0.php), [SDL2_image-devel-2.x.x-VC.zip](https://www.libsdl.org/projects/SDL_image/), [SDL2_mixer-devel-2.x.x-VC.zip](https://www.libsdl.org/projects/SDL_mixer/), [SDL2_ttf-devel-2.x.x-VC.zip](https://www.libsdl.org/projects/SDL_ttf/)
5. Open Developer Command Prompt for VS 2019 and run the following, substituting paths as appropriate:
```batch
SET SDL2DIR=C:\repos\breakhackBuild\SDL2-2.0.9
SET SDL2MIXERDIR=C:\repos\breakhackBuild\SDL2_mixer-2.0.4
SET SDL2_IMAGE=C:\repos\breakhackBuild\SDL2_image-2.0.4
SET SDLTTFDIR=C:\repos\breakhackBuild\SDL2_ttf-2.0.15
cmake -S C:\repos\breakhack -B C:\repos\breakhackBuild
cmake --build C:\repos\breakhackBuild --target package --config Release
```