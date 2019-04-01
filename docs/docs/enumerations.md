---
id: enumerations
title: Enumerations
sidebar_label: Enumerations
---

## `ClLogLevels`

### Description

The global maximum level of all log messages that will be recorded.

### Notes

- This is used for defining maximum level of messages that will actually get printed to the target output streams, not individual messages.
- The level that is set will be printed, along with all values numerically higher than it. For example, setting the level to CL_LOG_LEVEL_INFO will print all messages of types INFO, WARN, ERROR, and FATAL.
- Setting the value to CL_LOG_LEVEL_ALL is equivalent to CL_LOG_LEVEL_TRACE, and will print every log type, whereas setting it to CL_LOG_LEVEL_OFF will print nothing and disable logging.


## `ClMsgLevels`

### Description

Each logging level for a specific message.

### Notes

- This is only meant to be used when using the LOG() macro function above as the first parameter which describes the level you want a specific log message to be. When setting/getting the global log level, use ClLogLevels above.

## `ClStreams`

### Description

Enabling or disabling printing to the console, the disk, or both.

### Notes

- When printing to the disk is enabled, a valid filename must also be set using ClSetPropFilename() described below.
- When printing to the console is enabled, all messages of levels TRACE, DEBUG, INFO, and WARN will be printed to stdout, while ERROR and FATAL messages will be printed to stderr.

## `ClColor`

### Description

Enabling or disabling color printing to the console.

### Notes

- Color printing, if enabled, will only ever be used in messages printed to stdout and stderr.
- 