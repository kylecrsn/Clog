#ifndef CLOG_H
#define CLOG_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdarg.h>

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
#define LOG_FATAL(...) ClLog(CL_MSG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) ClLog(CL_MSG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)  ClLog(CL_MSG_LEVEL_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)  ClLog(CL_MSG_LEVEL_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...) ClLog(CL_MSG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_TRACE(...) ClLog(CL_MSG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)

/*
 * Description:
 * Macro function which allows the user to pass any level parameter of type ClMsgLevel to the 
 * internal logging function.
 * 
 * Warning: 
 * This is not the recommended way of logging info. While there are no mal-side-effects
 * of using this, it requires an extra parameter of what is normally an internal enumeration type 
 * and should instead only be used sparingly when conditional level-selection for a message is needed.
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
#define LOG(level, ...) ClLog(level, __FILE__, __LINE__, __VA_ARGS__)

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
 * Load the values from a user-declared instance of ClLogProps into the internal instance
 * 
 * Parameters:
 * - props:
 *    - Type: ClLogProps
 *    - Description: a user-defined instance of ClLogProps
 */
void ClLoadLogProps(ClLogProps props);

/*
 * Description:
 * Resets the values of the internal ClLogProps instance to the defaults.
 * 
 * Notes:
 * - This is effectively the same as calling ClInitProps() with the added caveat that it first 
 * checks if the internal file pointer is not NULL, and closes it if that is the case.
 */
void ClResetLogProps();

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
 * already exists, it will be appended to until the log rollover limit is reached.
 */
void ClSetPropFilename(char *filename);

/*
 * Description: Function that gets the filename to be used when logging to disk.
 */
char *ClGetPropFilename();

/*
 * [INTERNAL]
 * Description:
 * Enumeration describing each logging level.
 * 
 * Warning:
 * This is used internally by the library and should not be referenced in your code, unless you
 * are using the LOG() macro function.
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
 * [INTERNAL]
 * Description:
 * Function for recording the given message to the target output stream(s).
 * 
 * Warning:
 * This is used internally by the library and should not be referenced directly in your code. 
 * Instead use any of the macro functions defined above.
 */
void ClLog(ClMsgLevels level, const char *srcFilename, int srcLine, const char *message, ...);

#endif