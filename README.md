[![Build Status](https://travis-ci.org/Oliveshark/breakhack.svg?branch=master)](https://travis-ci.org/Oliveshark/breakhack)
[![Build status](https://ci.appveyor.com/api/projects/status/2a18npdntkmlx3dc?svg=true)](https://ci.appveyor.com/project/LiquidityC/breakhack)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/fc02d56fa7194e61b2c7d260fd2e4186)](https://www.codacy.com/app/LiquidityC/breakhack?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Oliveshark/breakhack&amp;utm_campaign=Badge_Grade)
[![CodeFactor](https://www.codefactor.io/repository/github/oliveshark/breakhack/badge/dev)](https://www.codefactor.io/repository/github/oliveshark/breakhack/overview/master)
<!--[![Coverity Scan](https://scan.coverity.com/projects/15218/badge.svg)](https://scan.coverity.com/projects/oliveshark-breakhack)-->
[![GitHub issues](https://img.shields.io/github/issues/oliveshark/breakhack.svg)](https://github.com/oliveshark/breakhack/issues)
[![GitHub closed issues](https://img.shields.io/github/issues-closed/oliveshark/breakhack.svg)](https://github.com/oliveshark/breakhack/issues)
![GitHub (pre-)release](https://img.shields.io/github/release/oliveshark/breakhack/all.svg)
[![Github All Releases](https://img.shields.io/github/downloads/oliveshark/breakhack/total.svg)](https://github.com/oliveshark/breakhack/releases)
[![HitCount](http://hits.dwyl.io/oliveshark/breakhack.svg)](http://hits.dwyl.io/oliveshark/breakhack)

Something in the ways of a roguelike
================================

A simple roguelike written in C using SDL2.

Idea
----
Randomly generated maps/rooms. Leveling system, permadeath,
multiple classes. Casual and fun for 10-20 minutes during a break
from regular work/play.

I wanted to practice C and I'm comfortable using SDL2. The assets
are graciously taken from the web. More info in the README in the
assets folder.

Download
--------
This game is available in the [steam store](https://store.steampowered.com/app/931040/BreakHack/) (release Okt 2018).
If you enjoy the game and want a properly installed, signed copy of the game with achievements and
global highscores you are most welcome to purchase it there.
If you feel like getting a free copy you can download and compile from here.

Issues
------
I'd prefer if issues could be handled here on github. Either check the list if your particular problem is reported
otherwise create a new issue. Support questions can be submitted as issues as well, I haven't figured out how I want to
work with support yet since I haven't received any questions.

License
-------
BreakHack is released under two different licenses.

- The source code is released under [GPLv3](https://github.com/oliveshark/breakhack/blob/master/LICENSE.txt)
- The Steam release is released under the [Steam subscriber agreement](https://store.steampowered.com/subscriber_agreement/)

Compile
-------
Apart from basic compile tools (*GCC/Clang, Make*) you'll also need to install **sdl2, sdl2-image, sdl2-ttf, sdl2-mixer** (If on a debian based dist you need to install the *dev* packages).
Optionally you can also install **cppcheck** and **physfs**

Once that is done run the following:
```bash
mkdir _build
cd _build
cmake -DCMAKE_BUILD_TYPE=Debug .. # Build type flag is optional
make
```

Also see [compilation instructions for Windows](MSVC_BUILD_INSTRUCTIONS.md).

Contribute
----------
Have an idea and able to implement it? Fork and send me a pull request and
I'll check it out. If I like it I will add it. If it's a big thing I'll add 
your name to the credits list (once I add it in game).

If you do contribute something you should be aware that if your PR is merged
into the master code-base your code will eventually make it out with the Steam version
of the game. I don't make any revenue from those sales so don't expect any other compensation
than a "I have code on Steam" feeling. Which is quite nice and also why I decided to release
on Steam.

Links
-----

- [Itch.io](https://store.steampowered.com/app/931040/BreakHack/)
- [Steam](https://liquidityc.itch.io/breakhack)
