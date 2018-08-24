#ifndef CLOG_H
#define CLOG_H

#include <stdio.h>
#include <stdarg.h>

/*
 * Description:
 * Macro functions corresponding to each of the different logging levels.
 * 
 * Parameters:
 * - message:
 *    - Type: char *
 *    - Required: true
 *    - Description: A string message with format specifies included. This functions similarly to
 * the format specifier string passed into functions like printf, fprintf, etc.
 * - (...):
 *    - Type: char *
 *    - Required: false
 *    - Description: One or more values of any type which correspond to the format specifier(s) in 
 * the message string.
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
 * Warning: This is not the recommended way of logging info. While there are no mal-side-effects
 * of using this, it requires an extra parameter of what is normally an internal enumeration type 
 * and should instead only be used sparingly when conditional level-selection for a message is needed.
 * 
 * Parameters:
 * - level:
 *    - Type: ClMsgLevels
 *    - Required: true
 *    - Description: the severity level of a specific message.
 * - message:
 *    - Type: char *
 *    - Required: true
 *    - Description: A string message with format specifies included. This functions similarly to
 * the format specifier string passed into functions like printf, fprintf, etc.
 * - (...):
 *    - Type: char *
 *    - Required: false
 *    - Description: One or more values of any type which correspond to the format specifier(s) in 
 * the message string.
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
typedef enum cl_log_level_e {
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
 * Enumeration enabling or disabling printing to the console.
 * 
 * Notes:
 * - When printing to the console, all messages of levels TRACE, DEBUG, INFO, and WARN will be 
 * printed to stdout, while all messages of levels ERROR and FATAL will be printed to stderr.
 */
typedef enum cl_console_e {
  CL_CONSOLE_OFF = 0,
  CL_CONSOLE_ON  = 1
} ClConsole;

/*
 * Description:
 * Enumeration enabling or disabling color printing to the console.
 * 
 * Notes:
 * - Color printing, if enabled, will only ever be used in messages printed to stdout and stderr.
 */
typedef enum cl_colors_e {
  CL_COLOR_OFF = 0,
  CL_COLOR_ON  = 1
} ClColor;

/*
 * Description:
 * Struct defining the properties(state) of the logging system.
 * 
 * Fields:
 * - FILE *fp: The file pointer used for recording messages to disk
 * - ClLogLevel level: See ClLogLevel
 * - ClConsole console: See ClConsole
 * - ClColor color: See ClColor
 * 
 * Notes:
 * - Using the initialization, load, reset, and getter/setter functions below, you probably won't 
 * need to declare an instance of this if you're just doing simple console or single-file  
 * logging. However, if your use-case is more complex, a useful pattern would be declaring multiple 
 * instances of this struct with different settings as needed, and loading them into clog's internal
 * log properties struct when relevant.
 */
typedef struct cl_log_props_s {
  FILE *        fp;
  ClLogLevels   level;
  ClConsole     console;
  ClColor       color;
} ClLogProps;

/*
 * Description:
 * Function for initializing the internal ClLogProps instance with default values.
 * 
 * Notes:
 * - Sets default values for the instance to:
 *    - fp: NULL
 *    - level: CL_LOG_LEVEL_INFO
 *    - console: CL_CONSOLE_ON
 *    - color: CL_COLOR_ON
 */
void ClInitLogProps();

/*
 * Description:
 * Load the values from a user-declared instance of ClLogProps into the internal instance
 * 
 * Parameters:
 * - props:
 *    - Type: ClLogProps
 *    - Required: true
 *    - Description: a user-defined instance of ClLogProps
 */
void ClLoadLogProps(ClLogProps props);

/*
 * Description:
 * Resets the values of the internal ClLogProps instance to the defaults.
 * 
 * Notes:
 * - This is effectively the same as calling ClInitProps(), and exists for clarity of intent's sake
 */
void ClResetLogProps();

/*
 * Description: Function that sets the file pointer used for recording messages to disk.
 * 
 * Parameters:
 * - fp:
 *    - Type: FILE *
 *    - Required: true
 *    - Description: The file descriptor corresponding to some already opened file controlled by the
 * user. The user is responsible for both properly opening and closing this pointer.
 */
void ClSetPropFp(FILE *fp);

/*
 * Description: Function that gets the file pointer used for recording messages to disk.
 */
FILE *ClGetPropFp();

/*
 * Description: Function that sets the global maximum level of log messages that will be recorded.
 * 
 * Parameters:
 * - level:
 *    - Type: ClLogLevels
 *    - Required: true
 *    - Description: See ClLogLevel
 */
void ClSetPropLevel(ClLogLevels level);

/*
 * Description: Function that gets the global maximum level of log messages that will be recorded.
 */
ClLogLevels ClGetPropLevel();

/*
 * Description: Function that sets whether or not console printing is enabled.
 * 
 * Parameters:
 * - console:
 *    - Type: ClConsole
 *    - Required: true
 *    - Description: See ClConsole
 */
void ClSetPropConsole(ClConsole console);

/*
 * Description: Function that gets whether or not console printing is enabled.
 */
ClConsole ClGetPropConsole();

/*
 * Description: Function that sets whether or not color for console printing is enabled.
 * 
 * Parameters:
 * - color:
 *    - Type: ClColor
 *    - Required: true
 *    - Description: See ClColor
 */
void ClSetPropColor(ClColor color);

/*
 * Description: Function that gets whether or not color for console printing is enabled.
 */
ClColor ClGetPropColor();

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
void ClLog(ClMsgLevels level, const char *filename, int line, const char *message, ...);

#endif