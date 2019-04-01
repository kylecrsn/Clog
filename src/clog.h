/*
  MIT License

  Copyright (c) 2018 Kyle Carson

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
 */

#ifndef CLOG_H
#define CLOG_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <time.h>
#include <pthread.h> 

/*
  ===============================================================================================
  WELCOME TO THE CLOG API v-

  Below you can find enumeration, struct, and function definitions for the Clog library. When 
  reading through the documentation top-to-bottom, you'll notice some types, functions, etc. are 
  referenced before they're defined, so feel free to look around if you see something you haven't 
  already read about. 
  
  An up-to-date, user-friendly, and more verbose version of this documentation can be found at:

  http://carsonkk.github.io/Clog

  Please note that the current version of the API documentation on the website may be more recent 
  than your version (specified in the title of this comment block), so be sure to select the 
  version that corresponds with your copy when using the site.

  If you're just getting started and would rather actually start to write code rather than 
  read all of the documentation in detail, check out the usage section of the website as it 
  fully covers how to use every feature the library offers through simple code snippets:

  http://carsonkk.github.io/Clog/usage

  If you find a bug or would like to suggest a feature, please open an issue on the Github page:

  https://github.com/carsonkk/Clog/issues

  Thanks for using Clog!
  ===============================================================================================
 */

#define CL_MIN_STREAM_LENGTH 1024

/*
  ==========================================================================================
  CLOG API: ENUMERATIONS

  Below are the enumerations that are meant to be used by the library's end-users
  ==========================================================================================
 */

/*
  DESCRIPTION:
  Enumeration describing the severity level of a message. The lower the numeric value, the more 
  severe the message should be.
 */
typedef enum cl_log_level_e {
  CL_LOG_LEVEL_FATAL = 0,
  CL_LOG_LEVEL_ERROR = 1,
  CL_LOG_LEVEL_WARN  = 2,
  CL_LOG_LEVEL_INFO  = 3,
  CL_LOG_LEVEL_DEBUG = 4,
  CL_LOG_LEVEL_TRACE = 5,
} ClLogLevel;

/*
  DESCRIPTION:
  Enumeration describing the type of stream the handler is logging to.
  
  NOTES:
  - When a handler's stream_type field is set to CL_STREAM_DISK, its name field must also be set 
  to either an existing filename or a new filename.
 */
typedef enum cl_stream_e {
  CL_STREAM_CONSOLE = 0,
  CL_STREAM_FILE    = 1,
  CL_STREAM_PIPE    = 2,
  CL_STREAM_STRING  = 3
} ClStream;

/*
  DESCRIPTION:
  Enumeration enabling or disabling the handler's ability to log messages.
 */
typedef enum cl_logging_e {
  CL_LOGGING_OFF = 0,
  CL_LOGGING_ON  = 1,
} ClLogging;

/*
  DESCRIPTION:
  Enumeration enabling or disabling SGR modifiers.
 */
typedef enum cl_sgr_e {
  CL_SGR_OFF = 0,
  CL_SGR_ON  = 1
} ClSgr;

typedef enum cl_format_type_e {
  CL_FORMAT_TYPE_STRING      = 0,
  CL_FORMAT_TYPE_MESSAGE     = 1,
  CL_FORMAT_TYPE_LEVEL       = 2,
  CL_FORMAT_TYPE_FILENAME    = 3,
  CL_FORMAT_TYPE_LINE_NUMBER = 4,
  CL_FORMAT_TYPE_FUNCTION    = 5,
  CL_FORMAT_TYPE_TIME        = 6,
  CL_FORMAT_TYPE_DURATION    = 7,
  CL_FORMAT_TYPE_ROLLOVER    = 8,
  CL_FORMAT_TYPE_PUBLIC_IP   = 9,
  CL_FORMAT_TYPE_PRIVATE_IP  = 10,
  CL_FORMAT_TYPE_PROC_ID     = 11,
  CL_FORMAT_TYPE_PROC_NAME   = 12,
  CL_FORMAT_TYPE_PROC_EXEC   = 13,
  CL_FORMAT_TYPE_PROC_USER   = 14,
  CL_FORMAT_TYPE_THREAD_ID   = 15,
  CL_FORMAT_TYPE_PTHREAD_ID  = 16,
  CL_FORMAT_TYPE_SGR_MODIFY  = 17,
  CL_FORMAT_TYPE_SGR_RESET   = 18
} ClFormatType;

typedef enum cl_sgr_type_e {
  CL_SGR_TYPE_BOLD             = 0,
  CL_SGR_TYPE_NO_BOLD          = 1,
  CL_SGR_TYPE_FAINT            = 2,
  CL_SGR_TYPE_NO_FAINT         = 3,
  CL_SGR_TYPE_ITALIC           = 4,
  CL_SGR_TYPE_NO_ITALIC        = 5,
  CL_SGR_TYPE_UNDERLINE        = 6,
  CL_SGR_TYPE_NO_UNDERLINE     = 7,
  CL_SGR_TYPE_STRIKETHROUGH    = 8,
  CL_SGR_TYPE_NO_STRIKETHROUGH = 9,
  CL_SGR_TYPE_REVERSE          = 10,
  CL_SGR_TYPE_NO_REVERSE       = 11,
  CL_SGR_TYPE_FG_COLOR         = 12,
  CL_SGR_TYPE_NO_FG_COLOR      = 13,
  CL_SGR_TYPE_BG_COLOR         = 14,
  CL_SGR_TYPE_NO_BG_COLOR      = 15,
} ClSgrType;

/*
  ==========================================================================================
  CLOG API: STRUCTS

  Below are the structs that are meant to be used by the library's end-users
  ==========================================================================================
 */


/*
  DESCRIPTION:
  Struct describing meta-information for a severity level.

  FIELDS:
  - level: The severity level represented as an integer.
  - level_string: The severity level represented as a string.
  - sgr_modifiers: Any SGR modifiers defined as a string of specifiers that is used when logging the 
  level_string (assuming the sgr_output field for the given handler is set to CL_SGR_ON).
  - sgr_resets: Resets for the modifiers specified in sgr_modifiers, defined as a string of 
  specifiers.
  - The parsed form of sgr_modifiers and sgr_resets concatenated around the level_string.
 */
typedef struct cl_level_s {
  ClLogLevel level;
  char *     level_string;
  char *     sgr_modifiers;
  char *     sgr_resets;
  char *     parsed_level;
} ClLevel;

typedef struct cl_format_part_s {
  ClFormatType  type;
  char *       context;
} ClFormatPart;

/*
  DESCRIPTION:
  Struct definition for a log handler. A handler contains all of the metadata related to a single 
  stream where information is to be logged. A "stream" can be the console (stdout/stderr), a file, 
  the write-end of a pipe, or a string (char *) buffer. While this interface may seem like a bit 
  much for the simple, single-log use case, it's benefits become clear with more complex use 
  cases in mind, such as:
  - Logging a single message to multiple streams
  - Contextually logging different messages to different streams
  - Customizing the properties of each stream
  Regardless of the use case, handlers provide a clear abstraction between what a user is writing 
  and where they want to write it.

  FIELDS:
  - id: A universally unique ID (UUID) identifying each handler instance.
  - logging: Enables or disables logging messages written to the handler.
  - name: The name of the stream (when the stream is a file).
  - extension: The extension of the stream (when the stream is a file).
  - filename
  - stream: A FILE pointer representing the stream.
  - stream_type: What type of descriptor the stream is.
  - stream_length: The current number of bytes that has been written to the stream.
  - stream_max_length: The maximum number of bytes that can be written to the stream.
  - format: A string representation of the format to be used when printing a message with the handler, 
  per Clog's format string specification.
  - parsed_format: The parsed version of the format field which is used internally.
  - parsed_format_length: The length of the parsed_format array field.
  - min_level: The most critical (numerically smallest) severity level the handler will log.
  - max_level: The least critical (numerically largest) severity level the handler will log.
  - rollover_count: the current number of times the log has rolled over its maximum length.
  - sgr_output: Enables or disables SGR text modifiers in the output.
  
  NOTES: 
  - By default, the library allocates two handlers automatically when ClInit() is called. These are 
  meant to provide sensible defaults for the user to get started with, and can be modified or deleted
  just like any other handler.
  - The name and rollover_count fields are only used when the stream_type field is set to 
  CL_STREAM_FILE.
  - The stream_length and stream_max_length fields are only used when the stream_type field is set 
  to CL_STREAM_FILE or CL_STREAM_STRING.
  - The min_level and max_level fields can define a range, or be the same for a single level, however 
  if their values are invalid (i.e. min_level is greater than max_level, etc.), the handler will 
  disable itself, as if the logging field was set to CL_LOGGING_OFF.
  - The sgr_output field is set to CL_SGR_OFF for all streams that aren't of type CL_STREAM_CONSOLE 
  by default. It can however be turned back on for non-console streams, which would cause the SGR 
  text modifiers to be printed in their raw, non-escaped format.
 */
typedef struct cl_handler_s {
  uuid_t        id;
  ClLogging     logging;
  int           fd;
  FILE *        fp;
  ClStream      stream_type;
  unsigned long stream_length;
  unsigned long stream_max_length;
  char *        name;
  char *        extension;
  char *        filename;
  unsigned long rollover_count;
  unsigned long rollover_max;
  ClSgr         sgr_output;
  char *        format;
  ClFormatPart *parsed_format;
  unsigned long parsed_format_length;
  ClLogLevel    min_level;
  ClLogLevel    max_level;
} ClHandler;

/*
  ===============================================================================================
  CLOG API: FUNCTIONS

  Below are the function declarations that are meant to be used by the library's end-users
  ===============================================================================================
 */

/*
  DESCRIPTION:
  Macro functions corresponding to each of the different logging severity levels.

  PARAMETERS:
  - ...:
    - TYPE: char *
    - DESCRIPTION: A format specifier message string with format specifies included, along with 
    zero or more values of any type which correspond to the format specifier(s) in the message 
    string. This functions similarly to the format specifier string + values passed into functions 
    such as printf(), fprintf(), etc.

  NOTES:
  - It is recommended that you use these functions as opposed to the LOG() function defined below 
  since they are explicit about the severity of the message per their names while saving you a 
  parameter.
  - Just like functions such as printf(), fprintf(), etc, if your optional arguments don't match your 
  format specifiers, or vice versa, the behavior is undefined and can result in garbage output.
 */
#define LOG_FATAL(...) ClLog(CL_LOG_LEVEL_FATAL, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ERROR(...) ClLog(CL_LOG_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_WARN(...)  ClLog(CL_LOG_LEVEL_WARN,  __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_INFO(...)  ClLog(CL_LOG_LEVEL_INFO,  __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_DEBUG(...) ClLog(CL_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_TRACE(...) ClLog(CL_LOG_LEVEL_TRACE, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

/*
  DESCRIPTION:
  Macro function which allows the user to pass any level of type ClLogLevel to the internal logging 
  function.
  
  PARAMETERS:
  - level:
     - TYPE: ClLogLevel
     - DESCRIPTION: the severity level of the message.
  - ...:
     - TYPE: char *
     - DESCRIPTION: A format specifier message string with format specifies included, along with 
     zero or more values of any type which correspond to the format specifier(s) in the message 
     string. This functions similarly to the format specifier string + values passed into functions 
     such as printf(), fprintf(), etc.
  
  NOTES:
  - It is recommended that you use the functions defined above as opposed to this function since 
  they are explicit about the severity of the log message per their names while saving you a 
  parameter.
 */
#define LOG(level, ...) ClLog(level, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

/*
  DESCRIPTION:
  Function for initializing the library and creating the default handlers. 
  
  The default handlers are:
  - Error Logging Handler:
    - id: <generated at runtime>
    - logging: CL_LOGGING_ON
    - name: NULL <not used>
    - stream: stderr
    - stream_type: CL_STREAM_CONSOLE
    - stream_length: 0 <not used>
    - stream_max_length: 0 <not used>
    - format: "%t(%Y-%m-%d%) %t(%H:%M:%S%) %l %g(%fK%)(%f:%L)%g(%F%): %m"
    - parsed_format: <generated at runtime>
    - parsed_format_length: <generated at runtime>
    - min_level: CL_LOG_LEVEL_FATAL
    - max_level: CL_LOG_LEVEL_ERROR
    - rollover_count: 0 <not used>
    - sgr_output: CL_SGR_ON
  - Output Logging Handler:
    - id: <generated at runtime>
    - logging: CL_LOGGING_ON
    - name: NULL <not used>
    - stream: stdout
    - stream_type: CL_STREAM_CONSOLE
    - stream_length: 0 <not used>
    - stream_max_length: 0 <not used>
    - format: "%t(%Y-%m-%d%) %t(%H:%M:%S%) %l %g(%fK%)(%f:%L)%g(%F%): %m"
    - parsed_format: <generated at runtime>
    - parsed_format_length: <generated at runtime>
    - min_level: CL_LOG_LEVEL_WARN
    - max_level: CL_LOG_LEVEL_TRACE
    - rollover_count: 0 <not used>
    - sgr_output: CL_SGR_ON
 */
void ClInit();

/*
  DESCRIPTION:
  Function that destroys all of the existing logging handlers by freeing any dynamically allocated 
  memory and closing any open file pointers within them.
 */
void ClCleanup();

/*
  DESCRIPTION:
  Resets the state of the library to its state after initialization had finished.
  
  NOTES:
  - This is effectively the same as calling ClCleanup() followed by ClInit().
 */
void ClReset();

ClHandler *ClCreateHandler(int fd, FILE *fp, ClStream stream_type, unsigned long stream_max_length, 
                           char *name, char *extension, unsigned long rollover_max, char *format, 
                           ClLogLevel min_level, ClLogLevel max_level);

void ClDeleteHandler(ClHandler *handler);

/*
  DESCRIPTION: Function that sets the format string to use for printing each log message.
  
  PARAMETERS:
  - format:
    - TYPE: char *
    - DESCRIPTION: A string of custom specifiers representing the format to be used when printing 
    each log message. Providing specifiers works similarly to how specifiers are written in C functions
    such as printf(), with a more simplified interface. The interface is the following, where everything
    within brackets is considered optional:

    %specifier[(%specifier[sub-specifier]%)]

    The common case with this interface is a percentage symbol followed by a specifier symbol. In some 
    cases, an optional pair of parantheses follow the specifier, in which another stream of specifiers 
    are meant to be given inside. In some cases, specifiers within parentheses will have a sub-specifier 
    that immediately follows, which indicates its actual value.
  
    The available specifiers are:
    - %m
      - NAME: Message
      - DESCRIPTION: The string(s) passed by the user as the actual message of the log entry.
    - %l
      - NAME: Level
      - DESCRIPTION: Severity level of the message.
    - %f
      - NAME: Filename
      - DESCRIPTION: The name of the file that the message was generated in.
    - %L
      - NAME: Line Number
      - DESCRIPTION: The line number in the file that the logging message was generated in.
    - %F
      - NAME: Function
      - DESCRIPTION: The function that the logging message was generated in.
    - %t(*%)
      - NAME: Time
      - DESCRIPTION: Any type of time-related text as described by strftime()'s specifiers, represented
      here as an asterisk. The contents of the parentheses get passed to strftime() as its format string.
    - %d
      - NAME: Duration
      - DESCRIPTION: The time since ClInit() was called. If the init function was called towards 
      the beginning of execution in a user's process (as it typically should be), this specifier's value
      can be used to estimate the duration of how long the process has been running.
    - %r
      - NAME: Log Rollover Count
      - DESCRIPTION: The current log rollover count, i.e. the number of times the log of the associated 
      handler has rolled over its maximum file length and created a new file.
    - %i
      - NAME: Public IP
      - DESCRIPTION: Public IP address of the machine in which the log message was generated.
    - %I
      - NAME: Private IP
      - DESCRIPTION: Private IP address of the machine in which the log message was generated.
    - %p
      - NAME: Process ID
      - DESCRIPTION: The id of the process in which the log message was generated.
    - %n
      - NAME: Process Name
      - DESCRIPTION: The name of the process in which the log message was generated.
    - %x
      - NAME: Process Executable
      - DESCRIPTION: The path to the process's executable in which the log message was generated.
    - %u
      - NAME: Process User
      - DESCRIPTION: The user that executed the process in which the log message was generated.
    - %T
      - NAME: Thread ID
      - DESCRIPTION: Using the Linux syscall gettid(), the id of the thread in which the 
      message was generated.
    - %P
      - NAME: Pthread ID
      - DESCRIPTION: Using the POSIX function pthread_self(), the id of the thread in which 
      the message was generated.
    - %g(*%)
      - NAME: SGR Text Modifiers
      - DESCRIPTION: Modifiers to change the way text is rendered when printed to a console output 
      stream. One or more specifiers listed below can be passed in lieu of the asterisk, and any text 
      which follows will be modified accordingly, until either the modifications are disabled with their
      corresponding disabling specifiers, or the catch-all reset specifier is used.
      - %d
        - NAME: Bold (Darker Intensity)
        - DESCRIPTION: Increases the "intensity" or boldness of the text.
      - %D
        - NAME: Disable Bold (Darker Intensity)
        - DESCRIPTION: Disables increasing the "intensity" or boldness of the text.
      - %l
        - NAME: Faint (Lighter Intensity)
        - DESCRIPTION: Decreases the "intensity" or boldness of the text.
      - %L
        - NAME: Disable Faint (Lighter Intensity)
        - DESCRIPTION: Disables decreasing the "intensity" or boldness of the text.
      - %i
        - NAME: Italic
        - DESCRIPTION: Italicizes the text. Note that this is not widely supported, and in some cases,
        is interpretted as "inverse the text" instead depending on your platform.
      - %I
        - NAME: Disable Italic
        - DESCRIPTION: Disables italicizing (or in some cases, inverting) the text.
      - %u
        - NAME: Underline
        - DESCRIPTION: Underlines the text.
      - %U
        - NAME: Disable Underline
        - DESCRIPTION: Disables underlining the text.
      - %s
        - NAME: Strikethrough
        - DESCRIPTION: Crosses out the text.
      - %S
        - NAME: Disable Strikethrough
        - DESCRIPTION: Disables crossing out the text.
      - %r
        - NAME: Reverse Colors
        - DESCRIPTION: Swaps the foreground and background colors of the text.
      - %R
        - NAME: Disable Reverse Colors
        - DESCRIPTION: Disables swapping the foreground and background colors of the text.
      - %f*
        - NAME: Foreground Color
        - DESCRIPTION: Sets the color of the text. A single sub-specifier below is meant to take the 
        place of the asterisk. For example, "%fY" would correspond to Bright Yellow. Colors are 
        described in terms of their 24-bit RGB values. You can use the predefined colors (letter 
        sub-specifiers), the colors from a user's Terminal color palette (integer sub-specifiers, 
        default colors listed, actual colors may differ if the user has a custom theme configured), 
        or a custom color (RGB sub-specifier).
        - k/K: Black   (0,0,0)       / Gray           (64,64,64)
        - w/W: White   (255,255,255) / Silver         (192,192,192)
        - r/R: Red     (128,0,0)     / Bright Red     (255,0,0)
        - o/O: Orange  (128,64,0)    / Bright Orange  (255,128,0)
        - y/Y: Yellow  (128,128,0)   / Bright Yellow  (255,255,0)
        - g/G: Green   (0,128,0)     / Bright Green   (0,255,0)
        - c/C: Cyan    (0,128,128)   / Bright Cyan    (0,255,255)
        - b/B: Blue    (0,0,128)     / Bright Blue    (0,0,255)
        - m/M: Magenta (128,0,128)   / Bright Magenta (255,0,255)
        - 1/9:  Black   / Bright Black
        - 2/10: Red     / Bright Red
        - 3/11: Green   / Bright Green
        - 4/12: Yellow  / Bright Yellow
        - 5/13: Blue    / Bright Blue
        - 6/14: Magenta / Bright Magenta
        - 7/15: Cyan    / Bright Cyan
        - 8/16: White   / Bright White
        - (r,g,b): Custom Color (Parantheses wrapping three comma-seperated values, no spaces, "r", 
        "g", and "b" should be integer values in the range of 0-255 inclusive)
      - %F
        - NAME: Disable Foreground Color
        - DESCRIPTION: Disables the set foreground color by reverting to the default text color.
      - %b*
        - NAME: Background Color
        - DESCRIPTION: Sets the color of the background around the text. Specifying the color is the 
        same as above for the foreground color.
      - %B
        - NAME: Disable Background Color
        - DESCRIPTION: Disables the set background color by reverting to the default background color.
    - %G
      - NAME: SGR Text Reset
      - DESCRIPTION: Disables all previously set SGR text modifiers by restoring all default values.
  
  NOTES:
  - If the character that immediately follows a percent symbol in the string isn't a valid specifier
  symbol, that percent symbol and any characters that follow are interrpreted literally, until a valid 
  specifier is given.
  - If an opening parentheses is specified without a closing parentheses (or vice versa), the percent
  symbol, specifier symbol, parantheses, and any other related characters are interpretted literally, 
  until a valid specifier is given.
  - To print a percent symbol and ensure it's interrpreted literally, write two percent symbols
  back to back. This should be used when you need to print a string literally which already corresponds 
  to one of the valid specifiers listed above.
  - For the time specifier, information on valid specifiers for inside of the parantheses can be found 
  in the relevant <time.h> documentation for strftime().
 */
void ClSetFormat(char *format);

/*
  DESCRIPTION: Function that gets format string to use for printing each log message.
 */
char *ClGetFormat();

/*
  DESCRIPTION:
  Set the SGR modifiers of the specified severity level. 
  
  When printing to a console stream, each severity level uses a default set of SGR modifiers. By 
  default, the modifiers include boldness, the user's default background color, and a custom
  foreground color depending on the severity level:
  
  FATAL: Bright Magenta (255,0,255)
  ERROR: Bright Red     (255,0,0)
  WARN:  Bright Yellow  (255,255,0)
  INFO:  Bright Green   (0,255,0)
  DEBUG: Bright Cyan    (0,255,255)
  TRACE: Bright Blue    (0,0,255)
  
  PARAMETERS:
  - level:
    - TYPE: ClLogLevel
    - DESCRIPTION: the severity level that you want to set the modifiers for.
  - sgr_modifiers:
    - TYPE: char *
    - DESCRIPTION: specifier(s), and their respective sub-specifier(s), for the various available SGR
    modifiers. Refer to the section above on Clog's interface for format specifiers and what modifiers 
    are available.
 */
void ClSetLevelSGR(ClLogLevel level, char *sgr_modifiers);

/*
  [INTERNAL]
  DESCRIPTION:
  Function for recording the given message to the target output stream(s).
  
  WARNING:
  This is used internally by the library and should NOT be referenced directly in your code. Instead, 
  use any of the macro functions defined at the top of this file. If you don't like how they're named, 
  or want to pass more info through them, feel free to edit them or define your own macro functions. 
  This function is only declared here (as opposed to being statically declared within clog.c) so the 
  macros can reference it in their definitions.
 */
void ClLog(ClLogLevel level, const char *filename, long line, const char *function, 
           const char *message, ...);

#endif
