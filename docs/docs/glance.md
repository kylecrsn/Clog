---
id: glance
title: Clog at a Glance
sidebar_label: Clog at a Glance
---

**Clog** is a logging library for C that's designed to be straightforward to use from the get-go. It provides a set of logging functions that are explicit in their intent by their declaration, along with a clear interface for setting and checking the properties used by the internal logging function. Similar to many other libraries, it has hierarchical log severity levels, along with many additional features such as colored console output and maximum file-size rollover.

While the library itself has a wide array of features, some of which quite customizable, no knowledge of these features is needed to get started with Clog.

## A Simple Introduction

The program below demonstrates how easy it is to begin using Clog:

```c
#include "clog.h"

int main(int argc, char **argv) {
  ClInitProps();

  CLOG_INFO("Hello World!");
  CLOG_WARN("This is a warning message...");
  CLOG_ERROR("...and this is an error message!");

  ClCleanupProps();
  return 0;
}
```

The call to `ClInitProps()` initializes the internal state used by the library to track things such as file handlers or what level of logs to record. This function *must* be called before any of the logging functions are used.

The next three functions each log a message. By default, two file handlers are used by Clog; one which sends error and fatal messages to `stderr`, and one which sends warning, information, debugging, and tracing messages to `stdout`.

Finally, `ClCleanProps()` is called to ensure any internally used memory is cleared. In this basic example, the library doesn't dynamically allocate memory or open extra file handlers, so calling this function isn't actually necessary. However, it's good practice to include it at the exit of your program as you may use features that rely on these things in the future.

Here's the output from the program:

![Output from the simple program above](assets/docusaurus.svg)

## Making Things a Bit More Interesting

Now let's say we want to log our messages to both the console and a file on disk. We also want to log messages of all severity levels, not just messages with a level of "info" and above. Finally, we want to include the state of some relevant variables whenever we log specific messages. Let's take a look at how we could do all of that using the [infamous FizzBuzz interview question](https://imranontech.com/2007/01/24/using-fizzbuzz-to-find-developers-who-grok-coding):

```c
#include "clog.h"

int main(int argc, char **argv) {
  int i;
  
  ClInitProps();

  ClAddHandler("fizzbuzz.log");
  ClSetPropLevel(CL_LOG_LEVEL_ALL);

  for(i = 1; i < 101; i++) {
    if(i%3 == 0 && i%5 == 0) {
      CLOG_INFO("FizzBuzz");
    }
    else if(i%3 == 0) {
      CLOG_INFO("Fizz");
    }
    else if(i%5 == 0) {
      CLOG_INFO("Buzz");
    }
    else {
      CLOG_TRACE("%d", i);
    }
  }

  ClCleanProps();
  return 0;
}
```

Just like last time, we start off by calling our initialization function.

Next we use `ClAddHandler()` to create a file handler. As a result, each message will now additionally be printed to the file named `fizzbuzz.log`. 

We also make a call to `ClSetPropLevel()`, which sets the global maximum severity level of messages that will be recorded. This is set to `CL_LOG_LEVEL_INFO` by default, which results in messages with an info level or higher being logged, while lower severity messages (i.e. debug and trace) are ignored and never recorded. By setting it to `CL_LOG_LEVEL_ALL`, we effectively enable the recording of all messages regardless of their severity level.

We then write the actual FizzBuzz output through calls to the info and trace logging functions. Some important things to note here include:

- When `i` is not divisible by three or five and we're meant to print out it's value, we leverage Clog's variadic implementation using format specifiers, in the same way you would print the state of variables using functions like `printf()`
- Since we changed the global log level to include every level, the trace messages with the value of `i` will be printed. This was purely to exemplify control over the logging level; we could have just as easily not messed with the default settings and used `CLOG_INFO` instead

Lastly, we call our cleanup function. This time it will actually have some functionality since we added a handler earlier, in that it will close the file descriptor that was previously opened for the file `fizzbuzz.log`.

Here is the output of the program, with the middle chunk redacted for brevity sake:

![Output from the FizzBuzz example above](assets/docusaurus.svg)

## Next Steps

The remainder of the Getting Started section discusses the installation options available for Clog and some miscellaneous considerations. The API section provides detailed documentation on each function and property. Finally, the Guides section outlines a feature-complete demonstration of the library with all kinds of useful examples to get you going on more advanced features.