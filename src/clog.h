#ifndef CLOG_H
#define CLOG_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

/*
 * Description:
 * Macro functions corresponding to each of the different logging levels.
 * 
 * Parameters:
 * - message:
 *    - Type: char *
 *    - Description: A string message with format specifies included. This functions similarly to
 *    the format specifier string passed into functions like printf, fprintf, etc.
 * - (...):
 *    - Type: char *
 *    - Required: false
 *    - Description: One or more values of any type which correspond to the format specifier(s) in 
 *    the message string.
 * 
 * Notes:
 * - It is recommended that you use these functions as they are explicit about the severity of the 
 * log message while saving you a parameter.
 * - Just like functions such as printf, fprintf, etc, if your optional arguments don't match your 
 * format specifiers, or vice versa, the behavior is undefined and can result in garbage output.
 */
#define LOG_FATAL(...) ClLog(CL_MSG_LEVEL_FATAL, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_ERROR(...) ClLog(CL_MSG_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_WARN(...)  ClLog(CL_MSG_LEVEL_WARN,  __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_INFO(...)  ClLog(CL_MSG_LEVEL_INFO,  __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_DEBUG(...) ClLog(CL_MSG_LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)
#define LOG_TRACE(...) ClLog(CL_MSG_LEVEL_TRACE, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

/*
 * Description:
 * Macro function which allows the user to pass any level parameter of type ClMsgLevel to the 
 * internal logging function.
 * 
 * Parameters:
 * - level:
 *    - Type: ClMsgLevels
 *    - Description: the severity level of a specific message.
 * - message:
 *    - Type: char *
 *    - Description: A string message with format specifies included. This functions similarly to
 * the format specifier string passed into functions like printf, fprintf, etc.
 * - (...):
 *    - Type: char *
 *    - Required: false
 *    - Description: One or more values of any type which correspond to the format specifier(s) in 
 *    the message string.
 */
#define LOG(level, ...) ClLog(level, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

/*
 * Description:
 * Enumeration describing the global maximum level of all log messages that will be recorded.
 * 
 * Notes:
 * - This is used for defining maximum level of messages that will actually get printed to the 
 * target output streams, not individual messages.
 * - The level that is set will be printed, along with all values numerically higher than it. For 
 * example, setting the level to CL_LOG_LEVEL_INFO will print all messages of types INFO, WARN, 
 * ERROR, and FATAL.
 * - Setting the value to CL_LOG_LEVEL_ALL is equivalent to CL_LOG_LEVEL_TRACE, and will print 
 * every log type, whereas setting it to CL_LOG_LEVEL_OFF will print nothing and disable logging.
 */
typedef enum cl_log_levels_e {
  CL_LOG_LEVEL_OFF   = -1,
  CL_LOG_LEVEL_FATAL = 0,
  CL_LOG_LEVEL_ERROR = 1,
  CL_LOG_LEVEL_WARN  = 2,
  CL_LOG_LEVEL_INFO  = 3,
  CL_LOG_LEVEL_DEBUG = 4,
  CL_LOG_LEVEL_TRACE = 5,
  CL_LOG_LEVEL_ALL   = CL_LOG_LEVEL_TRACE
} ClLogLevels;

/*
 * Description:
 * Enumeration describing each logging level for a specific message.
 * 
 * Notes:
 * - This is only meant to be used when using the LOG() macro function above as the first parameter 
 * which describes the level you want a specific log message to be. When setting/getting the global 
 * log level, use ClLogLevels above.
 */
typedef enum cl_msg_levels_e {
  CL_MSG_LEVEL_FATAL = CL_LOG_LEVEL_FATAL,
  CL_MSG_LEVEL_ERROR = CL_LOG_LEVEL_ERROR,
  CL_MSG_LEVEL_WARN  = CL_LOG_LEVEL_WARN,
  CL_MSG_LEVEL_INFO  = CL_LOG_LEVEL_INFO,
  CL_MSG_LEVEL_DEBUG = CL_LOG_LEVEL_DEBUG,
  CL_MSG_LEVEL_TRACE = CL_LOG_LEVEL_TRACE
} ClMsgLevels;

/*
 * Description:
 * Enumeration enabling or disabling printing to the console, the disk, or both.
 * 
 * Notes:
 * - When printing to the disk is enabled, a valid filename must also be set using 
 * ClSetPropFilename() described below.
 * - When printing to the console is enabled, all messages of levels TRACE, DEBUG, INFO, and WARN 
 * will be printed to stdout, while ERROR and FATAL messages will be printed to stderr.
 */
typedef enum cl_streams_e {
  CL_STREAM_OFF     = 0,
  CL_STREAM_CONSOLE = 1,
  CL_STREAM_DISK    = 2,
  CL_STREAM_ALL     = 3
} ClStreams;

/*
 * Description:
 * Enumeration enabling or disabling color printing to the console.
 * 
 * Notes:
 * - Color printing, if enabled, will only ever be used in messages printed to stdout and stderr.
 */
typedef enum cl_color_e {
  CL_COLOR_OFF = 0,
  CL_COLOR_ON  = 1
} ClColor;

/*
 * Description:
 * Struct defining the properties(state) of the logging system.
 * 
 * Fields:
 * - level: See ClLogLevel
 * - stream: See ClStreams
 * - color: See ClColor
 * - rollover: A soft cap on the maximum number of bytes the file that filename refers to
 * - filename: The name of the log file to write to
 * 
 * Notes:
 * - Using the initialization, load, reset, and getter/setter functions below, you probably won't 
 * need to declare an instance of this if you're just doing simple console or single-file  
 * logging. However, if your use-case is more complex, a useful pattern would be declaring multiple 
 * instances of this struct with different settings as needed, and loading them into clog's internal
 * log properties struct when relevant.
 * - For log rollover, if you have for example a file named "text.log" with rollover set to 2048, 
 * this will act as a "soft cap" on the maximum number of bytes the log file can be before 
 * rollover. If a logging call is made with a message that, when added to the file, would exceed 
 * this value, ClLog will:
 *    - Add the message to "text.log" as the final entry for that file.
 *    - Attempt to change the file's name to "text.log.1", appending the extra ".1". This will 
 *    succeed if it is the first time the log has rolled over, however if it has already rolled 
 *    over in the past, then there will already be a file with that name, in which case it will 
 *    increment the number it is trying to append to ".2" and so on.
 *    - Call fclose() on the internal file pointer and open a new file with the original filename 
 *    specified, using this for all future logging messages until the next rollover occurs.
 */
typedef struct cl_log_props_s {
  ClLogLevels level;
  ClStreams   stream;
  ClColor     color;
  long        rollover;
  char *      filename;
  char *      format;
} ClLogProps;

/*
 * Description:
 * Function for initializing the internal ClLogProps instance with default values.
 * 
 * Notes:
 * - Sets the default values for the instance to:
 *    - level: CL_LOG_LEVEL_INFO
 *    - stream: CL_STREAM_ALL
 *    - color: CL_COLOR_ON
 *    - rollover: 1048576 (1 MiB)
 *    - filename: NULL
 */
void ClInitLogProps();

/*
 * Description:
 * Frees any dynamically allocated memory and closes any open file pointers.
 */
void ClCleanupLogProps();

/*
 * Description:
 * Resets the values of the internal ClLogProps instance to the defaults.
 * 
 * Notes:
 * - This is effectively the same as calling ClCleanupLogProps() followed by ClInitProps().
 */
void ClResetLogProps();

/*
 * Description:
 * Load the values from a user-declared instance of ClLogProps into the internal instance
 * 
 * Parameters:
 * - props:
 *    - Type: ClLogProps
 *    - Description: a user-defined instance of ClLogProps
 */
void ClLoadLogProps(ClLogProps props);

/*
 * Description: Function that sets the global maximum level of log messages that will be recorded.
 * 
 * Parameters:
 * - level:
 *    - Type: ClLogLevels
 *    - Description: See ClLogLevel
 */
void ClSetPropLevel(ClLogLevels level);

/*
 * Description: Function that gets the global maximum level of log messages that will be recorded.
 */
ClLogLevels ClGetPropLevel();

/*
 * Description: Function that sets whether or not console and/or disk printing is enabled.
 * 
 * Parameters:
 * - stream:
 *    - Type: ClStreams
 *    - Description: See ClStreams
 */
void ClSetPropStream(ClStreams stream);

/*
 * Description: Function that gets whether or not console and/or disk printing is enabled.
 */
ClStreams ClGetPropStream();

/*
 * Description: Function that sets whether or not color for console printing is enabled.
 * 
 * Parameters:
 * - color:
 *    - Type: ClColor
 *    - Description: See ClColor
 */
void ClSetPropColor(ClColor color);

/*
 * Description: Function that gets whether or not color for console printing is enabled.
 */
ClColor ClGetPropColor();

/*
 * Description: Function that sets a soft cap on the log file rollover maximum limit size.
 * 
 * Parameters:
 * - rollover:
 *    - Type: long
 *    - Minimum: 1024
 *    - Maximum: LONG_MAX
 *    - Description: A soft cap on the maximum log file size which triggers rollover.
 * 
 * Notes:
 * - As mentioned above, this value is a soft cap, meaning that rollover to a new file occurs when 
 * after the first message that exceeds this limit is written to the file.
 * - Any value below 1024 will round up to 1024, and any value larger than LONG_MAX will round down 
 * to LONG_MAX.
 */
void ClSetPropRollover(long rollover);

/*
 * Description: Function that gets the log file rollover maximum limit size.
 */
long ClGetPropRollover();

/*
 * Description: Function that sets the filename to be used when logging to disk.
 * 
 * Parameters:
 * - filename:
 *    - Type: char *
 *    - Description: The filename corresponding to the target file for writing logs. If this file 
 *    already exists, it will be appended to until the log rollover limit is reached.
 */
void ClSetPropFilename(char *filename);

/*
 * Description: Function that gets the filename to be used when logging to disk.
 */
char *ClGetPropFilename();

/*
 * Description: Function that sets the format string to use for printing each log message.
 * 
 * Parameters:
 * - format:
 *    - Type: char *
 *    - Description: A string of custom specifiers representing the format to be used when printing 
 *    each message. The available specifiers are:
 * - %c*
 *    - Name: Text Color
 *    - Description: Indicates that any text following this specifier, until the Text Reset 
 *    specifier is found, should be of the color specified by the asterisk (*). The asterisk can be 
 *    any of the following:
 *        - k/K: Black/Bright Black
 *        - r/R: Red/Bright Red
 *        - g/G: Green/Bright Green
 *        - y/Y: Yellow/Bright Yellow
 *        - b/B: Blue/Bright Blue
 *        - m/M: Magenta/Bright Magenta
 *        - c/C: Cyan/Bright Cyan
 *        - w/W: White/Bright White
 * - %r
 *    - Name: Text Reset
 *    - Description: Disables any previously set text modifiers, including Text Color.
 * - %t{*}
 *    - Name: Time
 *    - Description: Any type of time-related text represented by strftime()'s specifiers, 
 *    specified here as an asterisk (*). The contents of the curly braces get passed to strftime() 
 *    as its format string, while the curly braces are consumed by the internal parsing function.
 * - %l
 *    - Name: Level
 *    - Description: Severity level of the message.
 * - %F
 *    - Name: Filename
 *    - Description: The name of the file that the message was generated in.
 * - %L
 *    - Name: Line
 *    - Description: The line number in the file that the logging message was generated in.
 * - %f
 *    - Name: Function
 *    - Description: The function in the file that the logging message was generated in.
 * - %p
 *    - Name: Process ID
 *    - Description: The process id of the process in which the log message was generated.
 * - %m
 *    - Name: Message
 *    - Description: The string(s) passed by the user as the actual message of the log entry.
 *
 * Notes:
 * - If the character that immediately follows a percent sign in the string isn't a valid 
 * specifier, that percent sign and its following character are interpretted literally.
 * - For a Text Color specifier, the actual color character should directly follow the specifier. 
 * For example, "%cG" indicates that all text that follows it should be Bright Green, until the 
 * Text Reset specifier is encountered. If no Text Reset specifier is encountered, all of the text 
 * that follows it would be Bright Green. If the color character that comes after the specifier is 
 * not a valid color character, the specifier is instead interpretted literally.
 * - For a Time specifier, an opening and closing curly brace should immediately follow the 
 * specifier, with the intent that anything in between them be passed to strftime(). For example, 
 * "%t{%Y-%m-%d}" would translate to "YYYY-MM-DD", or the year, month, and day of the logging 
 * message. Documentation on valid specifiers for inside of the curly braces can be found in any 
 * <time.h> documentation that supports it. If no closing curly brace is ever found, the specifier 
 * and opening curly brace are interpetted literally.
 */
void ClSetPropFormat(char *format);

/*
 * Description: Function that gets format string to use for printing each log message.
 */
char *ClGetPropFormat();

typedef enum cl_fmt_type_e {
  CL_FMT_TYPE_STRING     = 0,
  CL_FMT_TYPE_TEXT_COLOR = 1,
  CL_FMT_TYPE_TEXT_RESET = 2,
  CL_FMT_TYPE_TIME       = 3,
  CL_FMT_TYPE_LEVEL      = 4,
  CL_FMT_TYPE_FILENAME   = 5,
  CL_FMT_TYPE_LINE       = 6,
  CL_FMT_TYPE_FUNCTION   = 7,
  CL_FMT_TYPE_PROCESS_ID = 8,
  CL_FMT_TYPE_MESSAGE    = 9
} ClFmtType;

typedef struct cl_fmt_chunk_s {
  ClFmtType type;
  char *    str;
} ClFmtChunk;

/*
 * [INTERNAL]
 * Description:
 * Function for recording the given message to the target output stream(s).
 * 
 * Warning:
 * This is used internally by the library and should not be referenced directly in your code. 
 * Instead use any of the macro functions defined above.
 */
void ClLog(ClMsgLevels level, const char *filename, int line, 
           const char *function, const char *message, ...);

#endif