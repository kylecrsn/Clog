---
id: install
title: Compilation & Setup
sidebar_label: Compilation & Setup
---

There's a few different ways you can go about building Clog, depending on what the needs are for your project and what platform you're on.

## Drag & Drop

One option if you're just looking to try out Clog and aren't to worried about ensuring it's dependencies are already installed on your machine would be to just download the source files and include them in your project's compilation. These can be found at [`src/clog.c`]() and [`include/clog.h`](). While Clog itself doesn't depend on that many other libraries, the full list can be found at the beginning of the header file located [here]().

## Make

This options is more for if you just want to try out the programs in `tests/` and `examples/`, or verify that your version of Clog is building correctly.

```sh
git clone git@github.com:carsonkk/Clog.git
#git clone https://github.com/carsonkk/Clog.git
cd Clog
make
```

## CMake

With this option, Clog will be fully integrated into your machine and linked into your project as an external library.

TODO