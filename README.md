# Clog [![Build Status](https://travis-ci.org/carsonkk/Clog.svg?branch=master)](https://travis-ci.org/carsonkk/Clog)

> A simple, versatile logging library for C

## About

**Clog** is a logging library that's designed to be as straightforward as possible to use from the get-go. It avoids any sort of dynamic memory allocation and gives the user a clear interface for setting and checking the properties used by the internal logging function. It has a hierarchical log severity level similar to many other libraries, along with additional features such as colored console output and max file size rollover.

## Getting Started

In the simple example program below, we first initialize the properties for the internal ClLogProps instance to their defaults. This is all that's needed to start logging information to the console! By default, the default global severity level is INFO, so in order to demonstrate each logging function, we then set the level property to ALL.

```c
#include "clog.h"

int main(int argc, char *argv[]) {
  ClInitLogProps();
  ClSetPropLevel(CL_LOG_LEVEL_ALL);

  LOG_FATAL("This is a fatal message");
  LOG_ERROR("This is an error message");
  LOG_WARN("This is a warning message");
  LOG_INFO("This is an information message");
  LOG_DEBUG("This is a debugging message");
  LOG_TRACE("This is a tracing message");

  return 0;
}
```