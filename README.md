# Clog [![Build Status](https://travis-ci.org/carsonkk/Clog.svg?branch=master)](https://travis-ci.org/carsonkk/Clog)

> A simple, versatile logging library for C

## About

**Clog** is a logging library that's designed to be as straightforward as possible to use from the get-go. It avoids any sort of dynamic memory allocation and gives the user a clear interface for setting and checking the properties used by the internal logging function. It has a hierarchical log severity level similar to many other libraries, along with additional features such as colored console output and max file size rollover.

## Getting Started

The sample program below demonstrates nearly all of the functionality the library has to offer.

```c
#include "clog.h"

int main(int argc, char *argv[]) {
  int i;

  // Initialize the internal ClLogProps instance to sensible defaults. This is the first and only 
  // needed to get going.
  ClInitLogProps();

  // A simple message.
  LOG_INFO("This is a basic logging message.");

  // Format substituion similar to built-in functions like printf().
  LOG_INFO("This message includes %d substitutions, similar to how %s work.", 2, 
           "printf(), fprintf(), etc.");

  // Different colors for each severity level.
  LOG_FATAL("This is a fatal message with magenta coloring.");
  LOG_ERROR("This is an error message with red coloring.");
  LOG_WARN("This is a warning message with orange coloring.");
  LOG_INFO("This is an information message with green coloring.");

  // The default level property value is CL_LOG_LEVEL_INFO, thus this won't print.
  LOG_DEBUG("This is a debugging message (that never gets printed).");

  // Change the level property to print all levels of messages.
  ClSetPropLevel(CL_LOG_LEVEL_ALL);
  LOG_DEBUG("This is a debugging message with blue coloring.");
  LOG_TRACE("This is a tracing message with cyan coloring.");

  // Set a filename. Since the stream property is set to CL_STREAM_ALL by default, it will begin
  // printing to both the console and the file.
  ClSetPropFilename("demo.log");
  LOG_INFO("This will be printed to the console, as well as \"demo.log\" as its first entry.");

  // Now we'll change the stream property temporarily so we're only printing to the file, and 
  // change the rollover property to 1024 bytes. Then we'll force a log rollover to occur.
  ClSetPropStream(CL_STREAM_DISK);
  ClSetPropRollover(1024);
  for(i = 0; i < 7; i++) {
    LOG_INFO("This message is 116 characters long including the info that comes before it.");
  }
  LOG_INFO("This message will cause the file to exceed the rollover value, which triggers it to "
           "be closed and renamed with an incremental number appended to it. A new file with the "
           "original filename is then opened and used in its place.");
  LOG_INFO("This message will be the first message in the new log file.");

  ClSetPropStream(CL_STREAM_ALL);
  LOG_INFO("This concludes the feature demo for Clog!");

  // Cleanup the internal ClLogProps instance safely.
  ClResetLogProps();

  return 0;
}
```

All of these functions and more are available for getting, setting, and controlling the output of the library. For full documentation on the API check out `docs/`, and for more sample programs check out `examples/`.