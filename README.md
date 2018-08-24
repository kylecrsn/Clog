# clog [![Build Status](https://travis-ci.org/carsonkk/clog.svg?branch=master)](https://travis-ci.org/carsonkk/clog)

> An easy-to-use logging utility for C

## About

**clog** is a simple logging library that's designed to be as straightforward as possible to use from the get-go. To see how to get started, check out the example in the next section below.

## Getting Started

```c
#include "clog.h"

int main(int argc, char *argv[]) {

  // Initialize the global log properties
  cl_init_log_props();

  LOG_INFO("Check out this information");
}
```