[![Build Status](https://travis-ci.org/LiquidityC/breakhack.svg?branch=master)](https://travis-ci.org/LiquidityC/breakhack)
[![Build Status](https://ci.appveyor.com/api/projects/status/2nvna97cmm4cf535?svg=true)](https://ci.appveyor.com/project/LiquidityC/breakhack)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/fc02d56fa7194e61b2c7d260fd2e4186)](https://www.codacy.com/app/LiquidityC/breakhack?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=LiquidityC/breakhack&amp;utm_campaign=Badge_Grade)
[![CodeFactor](https://www.codefactor.io/repository/github/liquidityc/breakhack/badge/master)](https://www.codefactor.io/repository/github/liquidityc/breakhack/overview/master)
<!--[![Coverity Scan](https://scan.coverity.com/projects/15218/badge.svg)](https://scan.coverity.com/projects/liquidityc-breakhack)-->

Something in the ways of NETHACK
================================

A simple nethack similar game written in C using SDL2.

Idea
----
Randomly generated maps/rooms. Leveling system, permadeath,
multiple classes. Casual and fun for 10-20 minutes during a break
from regular work/play.

I wanted to practice C and I'm comfortable using SDL2. The assets
are graciously taken from the web. More info in the README in the
assets folder.

Future
------
I'm planning on releaseing this game on Steam once I'm happy with it.
Not to get rich, mostly because I think it will look nice on my resumé.
the code will remain open source under GPLv3 even after a possible Steam
release. So you'll still be able to download and compile it yourself if you
can and want to.

Compile
-------
Apart from basic compile tools (*GCC/Clang, Make*) you'll also need to install **lua, sdl2, sdl2-image, sdl2-ttf, sdl2-mixer** (If on a debian based dist you need to install the *dev* packages).
Optionally you can also install **cppcheck** and **physfs**

Once that is done run the following:
```bash
mkdir _build
cd _build
cmake -DCMAKE_BUILD_TYPE=Debug .. # Build type flag is optional
make
```

Contribute
----------
Have an idea and able to implement it? Fork and send me a pull request and
I'll check it out. If I like it I will add it. If it's a big thing I'll add 
your name to the credits list (once I add it in game).

Links
-----

- [https://liquidityc.itch.io/breakhack](https://liquidityc.itch.io/breakhack)
