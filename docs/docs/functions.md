---
id: functions
title: Functions
sidebar_label: Functions
---

Below are all of the functions exposed by Clog. A couple additional functions in the header file aren't included here; these are used internally by the library but need to be listed there.

## `void clog_fatal`
## `void clog_error`
## `void clog_warn`
## `void clog_info`
## `void clog_debug`
## `void clog_trace`

### Description

Macro functions corresponding to each of the different logging levels, used to record messages to the log.

### Parameters

#### `char *message`

- **Description**: A string message with format specifiers included. This functions similarly to the format specifier string passed as the first parameter to functions like `printf`, `fprintf`, etc.

#### `...`

- **Required**: false
- **Description**: One or more values of any type which correspond to the format specifier(s) in the `message` string parameter.

### Returns

Nothing.

> It is recommended that you use these functions as they are explicit about the severity of the log message while saving you a parameter.

> Just like functions such as `printf`, `fprintf`, etc, if your optional arguments don't match your format specifiers, or vice versa, the behavior is undefined and can result in garbage output.

## `void clog`

### Description

Macro function which allows the user to pass any level parameter of type `cl_msg_levels` to the internal logging function, along with a message to record to the log. The functions above can be thought of as a wrapper around this function, with predefined logging levels per their declarations.

### Parameters

#### `cl_msg_levels level`

- **Description**: The severity level of the given message.

#### `char *message`

- **Description**: A string message with format specifiers included. This functions similarly to the format specifier string passed as the first parameter to functions like `printf`, `fprintf`, etc.

#### `...`

- **Required**: false
- **Description**: One or more values of any type which correspond to the format specifier(s) in the `message` string parameter.

## Returns

Nothing.

> It is recommended that you use the logging functions above instead of this one as they are explicit about the severity of the log message while saving you a parameter.

> Just like functions such as `printf`, `fprintf`, etc, if your optional arguments don't match your format specifiers, or vice versa, the behavior is undefined and can result in garbage output.

## `void cl_init_props`

### Description

Initializes the propeties of the library. These properties are represented by a set of internal (static) variables in `clog.c`, as well as a set of properties exposed to, and controlled by, the user, stored in an instance of the [`cl_log_props`]() structure.

Sets default values of each property to:

- **level**: CL_LOG_LEVEL_INFO
- **stream**: CL_STREAM_ALL
- **color**: CL_COLOR_ON
- **rollover**: 1048576 (1 MiB)

Check [Enumerations]() for the details behind the relevant property values

### Parameters

None.

### Returns

Nothing.

## `cl_reset_props`

### Description

Resets the values of the internal ClLogProps values to their defaults.

### Parameters

None.

### Notes

- This is effectively the same as calling `cl_init_props` and exists for the sake of clarity when seting the properties to theri defaults later in execution.

## `void cl_cleanup()`

### Description

Frees any dynamically allocated memory and closes any open file pointers.

### Parameters

None.

## `void cl_load_props`

### Description

Load the values from a user-declared instance of `ClLogProps` into the internal instance.

### Parameters

#### `ClLogProps props`

- Description: A user-defined instance of `ClLogProps`.

## `void ClSetPropLevel`

### Description

Sets the global maximum level of log messages that will be recorded.

### Parameters

#### `ClLogLevels level`

- Description: Maximum global log level. *See ClLogLevel*

## `ClGetPropLevel`

### Description

Gets the global maximum level of log messages that will be recorded.

### Parameters

None.

## `void ClSetPropStream`

### Description

Sets whether or not console and/or disk printing is enabled.

### Parameters

#### `ClStreams stream`

- Description: Which streams to record on.

## `ClStreams ClGetPropStream`

### Description

Gets whether or not console and/or disk printing is enabled.

### Parameters

None.

## `void ClSetPropColor`

### Description

Sets whether or not color for console printing is enabled.

### Parameters

#### `ClColor color`

- Description: Enabled or disabled values.

## `ClColor ClGetPropColor`

### Description

Gets whether or not color for console printing is enabled.

### Parameters

None.

## `void ClSetPropRollover`

### Description

Sets a soft cap on the log file rollover maximum limit size.

### Parameters

#### `long rollover`

- Minimum: 1024
- Maximum: LONG_MAX
- Description: A soft cap on the maximum log file size which triggers rollover.

### Notes

- As mentioned above, this value is a soft cap, meaning that rollover to a new file occurs when after the first message that exceeds this limit is written to the file.
- Any value below 1024 will round up to 1024, and any value larger than LONG_MAX will round down to LONG_MAX.

## `long ClGetPropRollover`

### Description

Gets the log file rollover maximum limit size.

### Parameters

None.

## `void ClSetPropFilename`

### Description

Sets the filename to be used when logging to disk.

### Parameters

#### `char *filename`

- Description: The filename corresponding to the target file for writing logs. If this file already exists, it will be appended to until the log rollover limit is reached.

## `char *ClGetPropFilename`

### Description

Gets the filename to be used when logging to disk.

### Parameters

None.

## `void ClSetPropFormat`

### Description

Sets the format string to use for printing each log message.

### Parameters

#### `char *format`

- Description: A string of custom specifiers representing the format to be used when printing each message. The available specifiers are:

- %c*
  - Name: Text Color
  - Description: Indicates that any text following this specifier, until the Text Reset specifier is found, should be of the color specified by the asterisk (*). The asterisk can be any of the following:
    - k/K: Black/Bright Black
    - r/R: Red/Bright Red
    - g/G: Green/Bright Green
    - y/Y: Yellow/Bright Yellow
    - b/B: Blue/Bright Blue
    - m/M: Magenta/Bright Magenta
    - c/C: Cyan/Bright Cyan
    - w/W: White/Bright White
- %r
  - Name: Text Reset
  - Description: Disables any previously set text modifiers, including Text Color.
- %t{*}
  - Name: Time
  - Description: Any type of time-related text represented by strftime()'s specifiers, specified here as an asterisk (*). The contents of the curly braces get passed to strftime() as its format string, while the curly braces are consumed by the internal parsing function.
- %l
  - Name: Level
  - Description: Severity level of the message.
- %F
  - Name: Filename
  - Description: The name of the file that the message was generated in.
- %L
  - Name: Line
  - Description: The line number in the file that the logging message was generated in.
- %f
  - Name: Function
  - Description: The function in the file that the logging message was generated in.
- %p
  - Name: Process ID
  - Description: The process id of the process in which the log message was generated.
- %m
  - Name: Message
  - Description: The string(s) passed by the user as the actual message of the log entry.

### Notes

- If the character that immediately follows a percent sign in the string isn't a valid specifier, that percent sign and its following character are interpretted literally.
- For a Text Color specifier, the actual color character should directly follow the specifier. For example, "%cG" indicates that all text that follows it should be Bright Green, until the Text Reset specifier is encountered. If no Text Reset specifier is encountered, all of the text that follows it would be Bright Green. If the color character that comes after the specifier is not a valid color character, the specifier is instead interpretted literally.
- For a Time specifier, an opening and closing curly brace should immediately follow the specifier, with the intent that anything in between them be passed to strftime(). For example, "%t{%Y-%m-%d}" would translate to "YYYY-MM-DD", or the year, month, and day of the logging message. Documentation on valid specifiers for inside of the curly braces can be found in any `<time.h>` documentation that supports it. If no closing curly brace is ever found, the specifier and opening curly brace are interpetted literally.

## `char *ClGetPropFormat`

### Description

Gets format string to use for printing each log message.

### Parameters

None.
