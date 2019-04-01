---
id: structures
title: Structures
sidebar_label: Structures
---

## `ClLogProps`

### Description

Defining the properties(state) of the logging system.

### Fields

- level: See ClLogLevel
- stream: See ClStreams
- color: See ClColor
- rollover: A soft cap on the maximum number of bytes the file that filename refers to
- filename: The name of the log file to write to
  
### Notes

- Using the initialization, load, reset, and getter/setter functions below, you probably won't need to declare an instance of this if you're just doing simple console or single-file logging. However, if your use-case is more complex, a useful pattern would be declaring multiple instances of this struct with different settings as needed, and loading them into clog's internal log properties struct when relevant.
- For log rollover, if you have for example a file named "text.log" with rollover set to 2048, this will act as a "soft cap" on the maximum number of bytes the log file can be before rollover. If a logging call is made with a message that, when added to the file, would exceed this value, ClLog will:
  - Add the message to "text.log" as the final entry for that file.
  - Attempt to change the file's name to "text.log.1", appending the extra ".1". This will succeed if it is the first time the log has rolled over, however if it has already rolled over in the past, then there will already be a file with that name, in which case it will increment the number it is trying to append to ".2" and so on.
  - Call fclose() on the internal file pointer and open a new file with the original filename specified, using this for all future logging messages until the next rollover occurs.
