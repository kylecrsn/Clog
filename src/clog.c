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

#include "clog.h"

// SGR text and color modifier static constants
// https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters
static const char *sgr_reset                      = "\x1b[0m";
static const char *sgr_bold                       = "\x1b[1m";
static const char *sgr_no_bold                    = "\x1b[21m";
static const char *sgr_faint                      = "\x1b[2m";
static const char *sgr_no_faint                   = "\x1b[22m";
static const char *sgr_italic                     = "\x1b[3m";
static const char *sgr_no_italic                  = "\x1b[23m";
static const char *sgr_underline                  = "\x1b[4m";
static const char *sgr_no_underline               = "\x1b[24m";
static const char *sgr_strikethrough              = "\x1b[9m";
static const char *sgr_no_strikethrough           = "\x1b[29m";
static const char *sgr_reverse                    = "\x1b[7m";
static const char *sgr_no_reverse                 = "\x1b[27m";
static const char *sgr_fg_static_black            = "\x1b[38;2;0;0;0m";
static const char *sgr_fg_static_white            = "\x1b[38;2;255;255;255m";
static const char *sgr_fg_static_red              = "\x1b[38;2;128;0;0m";
static const char *sgr_fg_static_orange           = "\x1b[38;2;128;64;0m";
static const char *sgr_fg_static_yellow           = "\x1b[38;2;128;128;0m";
static const char *sgr_fg_static_green            = "\x1b[38;2;0;128;0m";
static const char *sgr_fg_static_cyan             = "\x1b[38;2;0;128;128m";
static const char *sgr_fg_static_blue             = "\x1b[38;2;0;0;128m";
static const char *sgr_fg_static_magenta          = "\x1b[38;2;128;0;128m";
static const char *sgr_fg_static_gray             = "\x1b[38;2;64;64;64m";
static const char *sgr_fg_static_silver           = "\x1b[38;2;192;192;192m";
static const char *sgr_fg_static_bright_red       = "\x1b[38;2;255;0;0m";
static const char *sgr_fg_static_bright_orange    = "\x1b[38;2;255;128;0m";
static const char *sgr_fg_static_bright_yellow    = "\x1b[38;2;255;255;0m";
static const char *sgr_fg_static_bright_green     = "\x1b[38;2;0;255;0m";
static const char *sgr_fg_static_bright_cyan      = "\x1b[38;2;0;255;255m";
static const char *sgr_fg_static_bright_blue      = "\x1b[38;2;0;0;255m";
static const char *sgr_fg_static_bright_magenta   = "\x1b[38;2;255;0;255m";
static const char *sgr_fg_variable_black          = "\x1b[30m";
static const char *sgr_fg_variable_white          = "\x1b[37m";
static const char *sgr_fg_variable_red            = "\x1b[31m";
static const char *sgr_fg_variable_yellow         = "\x1b[33m";
static const char *sgr_fg_variable_green          = "\x1b[32m";
static const char *sgr_fg_variable_cyan           = "\x1b[36m";
static const char *sgr_fg_variable_blue           = "\x1b[34m";
static const char *sgr_fg_variable_magenta        = "\x1b[35m";
static const char *sgr_fg_variable_bright_black   = "\x1b[90m";
static const char *sgr_fg_variable_bright_white   = "\x1b[97m";
static const char *sgr_fg_variable_bright_red     = "\x1b[91m";
static const char *sgr_fg_variable_bright_yellow  = "\x1b[93m";
static const char *sgr_fg_variable_bright_green   = "\x1b[92m";
static const char *sgr_fg_variable_bright_cyan    = "\x1b[96m";
static const char *sgr_fg_variable_bright_blue    = "\x1b[94m";
static const char *sgr_fg_variable_bright_magenta = "\x1b[95m";
static const char *sgr_fg_rgb_begin               = "\x1b[38;2;";
static const char *sgr_no_fg_color                = "\x1b[39m";
static const char *sgr_bg_static_black            = "\x1b[48;2;0;0;0m";
static const char *sgr_bg_static_white            = "\x1b[48;2;255;255;255m";
static const char *sgr_bg_static_red              = "\x1b[48;2;128;0;0m";
static const char *sgr_bg_static_orange           = "\x1b[48;2;128;64;0m";
static const char *sgr_bg_static_yellow           = "\x1b[48;2;128;128;0m";
static const char *sgr_bg_static_green            = "\x1b[48;2;0;128;0m";
static const char *sgr_bg_static_cyan             = "\x1b[48;2;0;128;128m";
static const char *sgr_bg_static_blue             = "\x1b[48;2;0;0;128m";
static const char *sgr_bg_static_magenta          = "\x1b[48;2;128;0;128m";
static const char *sgr_bg_static_gray             = "\x1b[48;2;64;64;64m";
static const char *sgr_bg_static_silver           = "\x1b[48;2;192;192;192m";
static const char *sgr_bg_static_bright_red       = "\x1b[48;2;255;0;0m";
static const char *sgr_bg_static_bright_orange    = "\x1b[48;2;255;128;0m";
static const char *sgr_bg_static_bright_yellow    = "\x1b[48;2;255;255;0m";
static const char *sgr_bg_static_bright_green     = "\x1b[48;2;0;255;0m";
static const char *sgr_bg_static_bright_cyan      = "\x1b[48;2;0;255;255m";
static const char *sgr_bg_static_bright_blue      = "\x1b[48;2;0;0;255m";
static const char *sgr_bg_static_bright_magenta   = "\x1b[48;2;255;0;255m";
static const char *sgr_bg_variable_black          = "\x1b[40m";
static const char *sgr_bg_variable_white          = "\x1b[47m";
static const char *sgr_bg_variable_red            = "\x1b[41m";
static const char *sgr_bg_variable_yellow         = "\x1b[43m";
static const char *sgr_bg_variable_green          = "\x1b[42m";
static const char *sgr_bg_variable_cyan           = "\x1b[46m";
static const char *sgr_bg_variable_blue           = "\x1b[44m";
static const char *sgr_bg_variable_magenta        = "\x1b[45m";
static const char *sgr_bg_variable_bright_black   = "\x1b[100m";
static const char *sgr_bg_variable_bright_white   = "\x1b[107m";
static const char *sgr_bg_variable_bright_red     = "\x1b[101m";
static const char *sgr_bg_variable_bright_yellow  = "\x1b[103m";
static const char *sgr_bg_variable_bright_green   = "\x1b[102m";
static const char *sgr_bg_variable_bright_cyan    = "\x1b[106m";
static const char *sgr_bg_variable_bright_blue    = "\x1b[104m";
static const char *sgr_bg_variable_bright_magenta = "\x1b[105m";
static const char *sgr_bg_rgb_begin               = "\x1b[48;2;";
static const char *sgr_no_bg_color                = "\x1b[49m";
static const char *sgr_rgb_a                      = ";";
static const char *sgr_rgb_m                      = "m";

// Misc default static constants
static const char *        default_name        = "clog";
static const char *        default_extension   = "log";
static const char *        default_format      = "%t(%Y-%m-%d%) %t(%H:%M:%S%) %l %g(%fK%)(%f:%L)%g(%F%): %m";
static const unsigned long default_level_count = 6;
static const ClLevel       default_levels[]    = {
  {
    CL_LOG_LEVEL_FATAL,
    "FATAL",
    "%d%fm",
    "%D%F"
  },
  {
    CL_LOG_LEVEL_ERROR,
    "ERROR",
    "%d%fr",
    "%D%F"
  },
  {
    CL_LOG_LEVEL_WARN,
    "WARN ",
    "%d%fy",
    "%D%F"
  },
  {
    CL_LOG_LEVEL_INFO,
    "INFO ",
    "%d%fg",
    "%D%F"
  },
  {
    CL_LOG_LEVEL_DEBUG,
    "DEBUG",
    "%d%fc",
    "%D%F"
  },
  {
    CL_LOG_LEVEL_TRACE,
    "TRACE",
    "%d%fb",
    "%D%F"
  }
};

// Misc static globals
static int           is_initialized  = 0;
static time_t        start_time      = 0;
static ClLevel *     levels          = NULL;
static ClHandler **  handlers        = NULL;
static unsigned long handlers_length = 0;

// Misc static helper functions
static unsigned long PrintMessage(ClHandler *handler, ClLogLevel level, const char *filename, 
                                  long line, const char *function, const char *message, va_list args);
static void ParseFormat(char *format, ClFormatPart **parsed_format, 
                        unsigned long *parsed_format_length);
static void CreateFormatParts(char *format, ClFormatPart **parsed_format, unsigned long *len, 
                              unsigned long i, unsigned long j);
static void CopyContext(char *format, ClFormatPart **parsed_format, unsigned long len, 
                        unsigned long i, unsigned long j);
static void ParseSgrModifiers(char *format, char **parsed_format, unsigned long i, unsigned long j);
static unsigned long ParseSgrModifierColors(char *format, char **parsed_format, unsigned long i);


void ClInit() {
  unsigned long i;

  if(is_initialized == 0) {
    is_initialized = 1;

    // Record the time when this function is first called
    time(&start_time);
  }
  
  // Generate the default severity levels
  levels = malloc(default_level_count*sizeof(ClLevel));
  for(i = 0; i < default_level_count; i++) {
    levels[i].level = default_levels[i].level;
    levels[i].level_string = malloc((strlen(default_levels[i].level_string)+1)*sizeof(char));
    strcpy(levels[i].level_string, default_levels[i].level_string);
    levels[i].sgr_modifiers = malloc((strlen(default_levels[i].sgr_modifiers)+1)*sizeof(char));
    strcpy(levels[i].sgr_modifiers, default_levels[i].sgr_modifiers);
    levels[i].sgr_resets = malloc((strlen(default_levels[i].sgr_resets)+1)*sizeof(char));
    strcpy(levels[i].sgr_resets, default_levels[i].sgr_resets);
    levels[i].parsed_level = malloc(sizeof(char));
    strcpy(levels[i].parsed_level, "");
    ParseSgrModifiers(levels[i].sgr_modifiers, &(levels[i].parsed_level), 0, strlen(levels[i].sgr_modifiers));
    levels[i].parsed_level = realloc(levels[i].parsed_level, 
                                     (strlen(levels[i].parsed_level)+
                                     strlen(default_levels[i].level_string)+1)*sizeof(char));
    strcat(levels[i].parsed_level, default_levels[i].level_string);
    ParseSgrModifiers(levels[i].sgr_resets, &(levels[i].parsed_level), 0, strlen(levels[i].sgr_resets));
  }

  // Create the two default log handlers and save pointers to them
  ClCreateHandler(0, stderr, CL_STREAM_CONSOLE, 0, NULL, NULL, 0, NULL, CL_LOG_LEVEL_FATAL, 
                  CL_LOG_LEVEL_ERROR);
  ClCreateHandler(0, stdout, CL_STREAM_CONSOLE, 0, NULL, NULL, 0, NULL, CL_LOG_LEVEL_INFO, 
                  CL_LOG_LEVEL_TRACE);
}


void ClCleanup() {
  unsigned long i;

  // Delete the levels
  for(i = 0; i < default_level_count; i++) {
    if(levels[i].level_string != NULL) {
      free(levels[i].level_string);
    }
    if(levels[i].sgr_modifiers != NULL) {
      free(levels[i].sgr_modifiers);
    }
    if(levels[i].sgr_resets != NULL) {
      free(levels[i].sgr_resets);
    }
  }
  free(levels);

  // Delete all of the allocated handlers
  for(i = 0; i < handlers_length; i++) {
    ClDeleteHandler(handlers[i]);
  }
}


void ClReset() {
  ClCleanup();
  ClInit();
}


ClHandler *ClCreateHandler(int fd, FILE *fp, ClStream stream_type, unsigned long stream_max_length, 
                           char *name, char *extension, unsigned long rollover_max, char *format, 
                           ClLogLevel min_level, ClLogLevel max_level) {
  ClHandler *handler = malloc(sizeof(ClHandler));

  // Generate a unique ID
  // TODO: Needs portability
  uuid_generate(handler->id);

  // Enable logging for all handler types by default
  handler->logging = CL_LOGGING_ON;

  // Configure the stream/file information depending on the type of stream
  handler->stream_type = stream_type;
  if(stream_type == CL_STREAM_CONSOLE) {
    if(fp != NULL) {
      handler->fp = fp;
    }
    else {
      // Invalid file pointer
      ClDeleteHandler(handler);
      return NULL;
    }

    // Enable SGR output by default
    handler->sgr_output = CL_SGR_ON;

    // Set unused fields to 0/NULL
    handler->fd = 0;
    handler->stream_length = 0;
    handler->stream_max_length = 0;
    handler->name = NULL;
    handler->extension = NULL;
    handler->filename = NULL;
    handler->rollover_count = 0;
    handler->rollover_max = 0;
  }
  else if(stream_type == CL_STREAM_FILE) {
    // If the stream type is a file but the user didn't specify a name or extension, set default ones
    if(name == NULL || strlen(name) == 0) {
      handler->name = malloc((1+strlen(default_name))*sizeof(char));
      strcpy(handler->name, default_name);
    }
    else {
      handler->name = malloc((1+strlen(name))*sizeof(char));
      strcpy(handler->name, name);
    }
    // Since the user may want a log without any extension, handle an empty extension string later
    if(extension == NULL) {
      handler->extension = malloc((1+strlen(default_extension))*sizeof(char));
      strcpy(handler->extension, default_extension);
    }
    else {
      handler->extension = malloc((1+strlen(extension))*sizeof(char));
      strcpy(handler->extension, extension);
    }

    // Use the name and extension to create the filename. If the extension was specified as an empty
    // string, then assume the user does not want an extension i.e. the filename is just the name
    if(extension != NULL && strlen(extension) == 0) {
      handler->filename = malloc((1+strlen(handler->name))*sizeof(char));
      strcpy(handler->filename, handler->name);
    }
    else {
      handler->filename = malloc((2+strlen(handler->name)+strlen(handler->extension))*sizeof(char));
      sprintf(handler->filename, "%s.%s", name, extension);
    }
    handler->fp = fopen(handler->filename, "a");
    
    // Set stream_length to the current EOF
    handler->stream_length = (unsigned long)ftell(handler->fp);
    // Set stream_max_length to the maximum size the file can be in bytes before rollover occurs
    if(stream_max_length < CL_MIN_STREAM_LENGTH) {
      handler->stream_max_length = CL_MIN_STREAM_LENGTH;
    }
    else if(stream_max_length > ULONG_MAX) {
      handler->stream_max_length = ULONG_MAX;
    }
    else {
      handler->stream_max_length = stream_max_length;
    }

    // TODO: handle pre-existing rollovers
    handler->rollover_count = 0;
    if(rollover_max < 0) {
      handler->rollover_max = 0;
    }
    else if(rollover_max > ULONG_MAX) {
      handler->rollover_max = ULONG_MAX;
    }
    else {
      handler->rollover_max = rollover_max;
    }

    // Disable SGR output by default
    handler->sgr_output = CL_SGR_OFF;

    // Set unused fields to 0/NULL
    handler->fd = 0;
  }
  else if(stream_type == CL_STREAM_PIPE) {
    // Convert the pipe's write file descriptor into a file pointer struct
    // TODO: This is POSIX only, needs portability
    // C89: Use pipe writing function calls when writing the message, no fp needed
    // handler->fp = fdopen(fd, "a");

    // Store the pipe's write-end file descriptor
    handler->fd = fd;

    handler->stream_length = 0;
    // Set stream_max_length to the maximum size the pipe buffer can be in bytes
    if(stream_max_length < CL_MIN_STREAM_LENGTH) {
      handler->stream_max_length = CL_MIN_STREAM_LENGTH;
    }
    else if(stream_max_length > ULONG_MAX) {
      handler->stream_max_length = ULONG_MAX;
    }
    else {
      handler->stream_max_length = stream_max_length;
    }

    // Disable SGR output by default
    handler->sgr_output = CL_SGR_OFF;

    // Set unused fields to 0/NULL
    handler->fp = NULL;
    handler->name = NULL;
    handler->extension = NULL;
    handler->filename = NULL;
    handler->rollover_count = 0;
    handler->rollover_max = 0;
  }
  else if(stream_type == CL_STREAM_STRING) {
    // Open a stream in memory that treats a string buffer as a file pointer
    // TODO: This is POSIX only, needs portability
    // C89: Create a temporary file or use string appends

    // TODO
    // http://man7.org/linux/man-pages/man3/fmemopen.3.html
  }
  else {
    // Invalid stream_type
    // TODO: Error code mechanism?
    ClDeleteHandler(handler);
    return NULL;
  }

  // Store and parse the format
  if(format == NULL || strlen(format) == 0) {
    handler->format = malloc((1+strlen(default_format))*sizeof(char));
    strcpy(handler->format, default_format);
  }
  else {
    handler->format = malloc((1+strlen(format))*sizeof(char));
    strcpy(handler->format, format);
  }
  ParseFormat(handler->format, &(handler->parsed_format), &(handler->parsed_format_length));

  // Set the logging level range
  if(min_level < CL_LOG_LEVEL_FATAL || min_level > CL_LOG_LEVEL_TRACE) {
    ClDeleteHandler(handler);
    return NULL;
  }
  else if(max_level < CL_LOG_LEVEL_FATAL || max_level > CL_LOG_LEVEL_TRACE) {
    ClDeleteHandler(handler);
    return NULL;
  }
  else if(min_level > max_level) {
    ClDeleteHandler(handler);
    return NULL;
  }
  handler->min_level = min_level;
  handler->max_level = max_level;

  // Save and return the handler pointer
  handlers_length++;
  handlers = (handlers_length == 1) ? 
             malloc(sizeof(ClHandler *)) : 
             realloc(handlers, handlers_length*sizeof(ClHandler *));
  handlers[handlers_length-1] = handler;
  return handler;
}


void ClDeleteHandler(ClHandler *handler) {
  unsigned long i;

  if(handler->fp != NULL) {
    fclose(handler->fp);
    handler->fp = NULL;
  }
  if(handler->name != NULL) {
    free(handler->name);
  }
  if(handler->extension != NULL) {
    free(handler->extension);
  }
  if(handler->filename != NULL) {
    free(handler->filename);
  }
  if(handler->format != NULL) {
    free(handler->format);
  }
  if(handler->parsed_format != NULL && handler->parsed_format_length != 0) {
    for(i = 0; i < handler->parsed_format_length; i++) {
      if(handler->parsed_format[i].context != NULL) {
        free(handler->parsed_format[i].context);
      }
    }
    free(handler->parsed_format);
  }
  free(handler);
}

// TODO: setters and getters (customize level and handler struct fields)

// TODO: function (__FUNCTION__ or __func__) is not portable
void ClLog(ClLogLevel level, const char *filename, long line, const char *function, 
           const char *message, ...) {
  unsigned long i;
  unsigned long fn_len;
  char *        fn_rolled;
  FILE *        tp;
  va_list       args;

  for(i = 0; i < handlers_length; i++) {
    if(level >= handlers[i]->min_level && level <= handlers[i]->max_level) {
      // Handle different methods of printing depending on the stream
      switch(handlers[i]->stream_type) {
        case CL_STREAM_CONSOLE:
          va_start(args, message);
          PrintMessage(handlers[i], level, filename, line, function, message, args);
          va_end(args);
          break;
        case CL_STREAM_FILE:
          va_start(args, message);
          handlers[i]->stream_length += PrintMessage(handlers[i], level, filename, line, function, 
                                                       message, args);
          va_end(args);

          // Perform log rollover if necessary
          if(handlers[i]->stream_length > handlers[i]->stream_max_length) {
            fn_len = (unsigned long)strlen(handlers[i]->filename);
            while(1) {
              // Set fn_rolled to be the new rollover file
              fn_rolled = malloc((fn_len+3+(int)floor(log10(handlers[i]->rollover_count)))*sizeof(char));
              sprintf(fn_rolled, "%s.%ld", handlers[i]->filename, handlers[i]->rollover_count);

              // Try opening the new rollover file to see if it already exists
              tp = fopen(fn_rolled, "r");
              if(tp != NULL) {
                // Already exists, free memory and check the next largest rollover number
                fclose(tp);
                tp = NULL;
                free(fn_rolled);
                handlers[i]->rollover_count++;
              }
              else {
                // Doesn't exists, close and rename the current file to the rolled-over name
                fclose(handlers[i]->fp);
                handlers[i]->fp = NULL;
                rename(handlers[i]->filename, fn_rolled);

                // Create a new empty file with the regular filename to log future messages to
                handlers[i]->fp = fopen(handlers[i]->filename, "a");
                handlers[i]->rollover_count++;
                handlers[i]->stream_length = 0;
                break;
              }
            }
          }
          break;
        case CL_STREAM_PIPE:
          // TODO
          break;
        case CL_STREAM_STRING:
          // TODO
          break;
        default:
          // TODO: error mechanism?
          break;
      }
    }
  }
}


static unsigned long PrintMessage(ClHandler *handler, ClLogLevel level, const char *filename, 
                                  long line, const char *function, const char *message, va_list args) {
  unsigned long i;
  unsigned long len = 0;
  unsigned long tm_len = 0;
  unsigned long tm_max = 64;
  char *        tm;
  time_t        raw_time;

  for(i = 0; i < handler->parsed_format_length; i++) {
    switch(handler->parsed_format[i].type) {
      case CL_FORMAT_TYPE_STRING:
      case CL_FORMAT_TYPE_SGR_MODIFY:
      case CL_FORMAT_TYPE_SGR_RESET:
        len += fprintf(handler->fp, "%s", handler->parsed_format[i].context);
        break;
      case CL_FORMAT_TYPE_MESSAGE:
        len += vfprintf(handler->fp, message, args);
        break;
      case CL_FORMAT_TYPE_LEVEL:
        len += fprintf(handler->fp, "%s", levels[level].parsed_level);
        break;
      case CL_FORMAT_TYPE_FILENAME:
        len += fprintf(handler->fp, "%s", filename);
        break;
      case CL_FORMAT_TYPE_LINE_NUMBER:
        len += fprintf(handler->fp, "%ld", line);
        break;
      case CL_FORMAT_TYPE_FUNCTION:
        len += fprintf(handler->fp, "%s", function);
        break;
      case CL_FORMAT_TYPE_TIME:
        tm = malloc(tm_max*sizeof(char));
        time(&raw_time);
        do {
          tm_len = strftime(tm, tm_max, handler->parsed_format[i].context, localtime(&raw_time));
          if(tm_len == 0) {
            tm_max *= 2;
            tm = realloc(tm, tm_max*sizeof(char));
          }
        } while(tm_len == 0);
        len += fprintf(handler->fp, "%s", tm);
        free(tm);
        break;
      case CL_FORMAT_TYPE_DURATION:
        // TODO: strftime like function to print duration in terms of weeks/days/hours/minutes/seconds
        break;
      case CL_FORMAT_TYPE_ROLLOVER:
        len += fprintf(handler->fp, "%ld", handler->rollover_count);
        break;
      case CL_FORMAT_TYPE_PUBLIC_IP:
        // TODO
        break;
      case CL_FORMAT_TYPE_PRIVATE_IP:
        // TODO
        break;
      case CL_FORMAT_TYPE_PROC_ID:
        // TODO
        break;
      case CL_FORMAT_TYPE_PROC_NAME:
        // TODO
        break;
      case CL_FORMAT_TYPE_PROC_EXEC:
        // TODO
        break;
      case CL_FORMAT_TYPE_PROC_USER:
        // TODO
        break;
      case CL_FORMAT_TYPE_THREAD_ID:
        // TODO: portability
        len += fprintf(handler->fp, "%d", (pid_t)syscall(SYS_gettid));
        break;
      case CL_FORMAT_TYPE_PTHREAD_ID:
        // TODO: Not portable, maybe allow the user to pass a function pointer for this?
        // https://stackoverflow.com/questions/34370172/the-thread-id-returned-by-pthread-self-is-not-the-same-thing-as-the-kernel-thr
        len += fprintf(handler->fp, "%ld", (long)pthread_self());
        break;
      default:
        // TODO: need to handle? no?
        break;
    }
  }

  len += fprintf(handler->fp, "\n");
  fflush(handler->fp);
  return len;
}


static void ParseFormat(char *format, ClFormatPart **parsed_format, 
                        unsigned long *parsed_format_length) {
  unsigned long i;
  unsigned long j;
  unsigned long old_i;
  unsigned long old_j;
  unsigned long len = 0;
  unsigned long format_len = (unsigned long)strlen(format);

  // Create the format part(s) that correspond to each specifier, as well as each static string of 
  // characters within the format string. i and j start at the beginning of the string and i is 
  // incremented until a specifier is found, at which point the difference between i and j is used 
  // to capture the static string preceeding the specifier, then the specifier is captured, then j   
  // is set to the position just after the specifier to prepare for capturing the next sequence.
  for(i = 0, j = 0; i < format_len; i++) {
    if(format[i] == '%') {
      switch(format[i+1]) {
        case 'm':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_MESSAGE;
          (*parsed_format)[len].context = NULL;
          len++;
          // Set i to to be the specifier symbol (in this case the 'L') and set j to be the 
          // character that follows it
          i++;
          j = i + 1;
          break;
        case 'l':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_LEVEL;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'f':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_FILENAME;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'L':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_LINE_NUMBER;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'F':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_FUNCTION;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 't':
          if(format[i+2] == '(') {
            // Keep track of the current positions of i and j
            old_i = i;
            old_j = j;

            // Set the new positions to be the beginning of the string within the parentheses
            i += 3;
            j = i;

            // Find the closing parenthese specifier
            while(format[i] != '\0') {
              if(format[i] == ')' && format[i-1] == '%') {
                break;
              }
              i++;
            }

            // If it was found, store the context to use when printing
            if(format[i] == ')') {
              CreateFormatParts(format, parsed_format, &len, old_i, old_j);
              (*parsed_format)[len].type = CL_FORMAT_TYPE_TIME;
              CopyContext(format, parsed_format, len, i-1, j);
              len++;
              j = i + 1;
            }
            // Otherwise the end of the string was found, ignore and continue parsing
            else {
              i = old_i;
              j = old_j;
            }
          }
          break;
        case 'd':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_DURATION;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'r':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_ROLLOVER;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'i':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_PUBLIC_IP;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'I':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_PRIVATE_IP;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'p':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_PROC_ID;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'n':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_PROC_NAME;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'x':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_PROC_EXEC;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'u':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_PROC_USER;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'T':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_THREAD_ID;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'P':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_PTHREAD_ID;
          (*parsed_format)[len].context = NULL;
          len++;
          i++;
          j = i + 1;
          break;
        case 'g':
          if(format[i+2] == '(') {
            old_i = i;
            old_j = j;
            i += 3;
            j = i;
            while(format[i] != '\0') {
              if(format[i] == ')' && format[i-1] == '%') {
                break;
              }
              i++;
            }
            if(format[i] == ')') {
              CreateFormatParts(format, parsed_format, &len, old_i, old_j);
              (*parsed_format)[len].type = CL_FORMAT_TYPE_SGR_MODIFY;
              (*parsed_format)[len].context = malloc(sizeof(char));
              strcpy((*parsed_format)[len].context, "");
              ParseSgrModifiers(format, &((*parsed_format)[len].context), j, i-1);
              if(strlen((*parsed_format)[len].context) == 0) {
                free((*parsed_format)[len].context);
                (*parsed_format)[len].context = NULL;
              }
              len++;
              j = i + 1;
            }
            else {
              i = old_i;
              j = old_j;
            }
          }
          break;
        case 'G':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_SGR_RESET;
          (*parsed_format)[len].context = malloc((strlen(sgr_reset)+1)*sizeof(char));
          strcpy((*parsed_format)[len].context, sgr_reset);
          len++;
          i++;
          j = i + 1;
          break;
        case '%':
          CreateFormatParts(format, parsed_format, &len, i, j);
          (*parsed_format)[len].type = CL_FORMAT_TYPE_STRING;
          (*parsed_format)[len].context = malloc(2*sizeof(char));
          strcpy((*parsed_format)[len].context, "%%");
          len++;
          i++;
          j = i + 1;
          break;
        default:
          // TODO?
          break;
      }
    }
  }

  // If there is a static string at the end of the format string, parse it
  CreateFormatParts(format, parsed_format, &len, i, j);
  
  *parsed_format_length = len;
}


static void CreateFormatParts(char *format, ClFormatPart **parsed_format, unsigned long *len, 
                              unsigned long i, unsigned long j) {
  // If i and j aren't the same, there are one or more characters that make up a static string
  // which needs to be parsed before the specifier
  if(i-j > 0) {
    // When the static string is the last sequence of characters in the format, with no specifiers 
    // following it
    if(i == strlen(format)) {
      *parsed_format = (*len == 0) ? 
                      malloc(sizeof(ClFormatPart)) : 
                      realloc(*parsed_format, (*len+1)*sizeof(ClFormatPart));
      (*parsed_format)[*len].type = CL_FORMAT_TYPE_STRING;
    }
    // When there is at least one specifier (or more) following the static string
    else {
      *parsed_format = (*len == 0) ? 
                      malloc(2*sizeof(ClFormatPart)) : 
                      realloc(*parsed_format, (*len+2)*sizeof(ClFormatPart));
      (*parsed_format)[*len].type = CL_FORMAT_TYPE_STRING;
    }

    // Copy over the static string to the format part
    CopyContext(format, parsed_format, *len, i, j);
    *len += 1;
  }
  else {
    *parsed_format = (*len == 0) ? 
                    malloc(sizeof(ClFormatPart)) : 
                    realloc(*parsed_format, (*len+1)*sizeof(ClFormatPart));
  }
}


static void CopyContext(char *format, ClFormatPart **parsed_format, unsigned long len, 
                        unsigned long i, unsigned long j) {
  (*parsed_format)[len].context = malloc((i-j+1)*sizeof(char));
  strncpy((*parsed_format)[len].context, format+j, i-j);
  (*parsed_format)[len].context[i-j] = '\0';
}


static void ParseSgrModifiers(char *format, char **parsed_format, unsigned long i, unsigned long j) {
  while(i < j) {
    if(format[i] == '%') {
      switch(format[i+1]) {
        case 'd':
          // Increase the size of the parsed_format string to be large enough to store the static 
          // string that corresponds to the specifier.
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_bold)+1)*sizeof(char));
          strcat(*parsed_format, sgr_bold);

          // Set i to to be the specifier symbol (in this case the 'd') and set j to be the 
          // character that follows it
          i++;
          break;
        case 'D':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_no_bold)+1)*sizeof(char));
          strcat(*parsed_format, sgr_no_bold);
          i++;
          break;
        case 'l':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_faint)+1)*sizeof(char));
          strcat(*parsed_format, sgr_faint);
          i++;
          break;
        case 'L':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_no_faint)+1)*sizeof(char));
          strcat(*parsed_format, sgr_no_faint);
          i++;
          break;
        case 'i':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_italic)+1)*sizeof(char));
          strcat(*parsed_format, sgr_italic);
          i++;
          break;
        case 'I':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_no_italic)+1)*sizeof(char));
          strcat(*parsed_format, sgr_no_italic);
          i++;
          break;
        case 'u':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_underline)+1)*sizeof(char));
          strcat(*parsed_format, sgr_underline);
          i++;
          break;
        case 'U':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_no_underline)+1)*sizeof(char));
          strcat(*parsed_format, sgr_no_underline);
          i++;
          break;
        case 's':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_strikethrough)+1)*sizeof(char));
          strcat(*parsed_format, sgr_strikethrough);
          i++;
          break;
        case 'S':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_no_strikethrough)+1)*sizeof(char));
          strcat(*parsed_format, sgr_no_strikethrough);
          i++;
          break;
        case 'r':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_reverse)+1)*sizeof(char));
          strcat(*parsed_format, sgr_reverse);
          i++;
          break;
        case 'R':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_no_reverse)+1)*sizeof(char));
          strcat(*parsed_format, sgr_no_reverse);
          i++;
          break;
        case 'f':
          i = ParseSgrModifierColors(format, parsed_format, i+1);
          break;
        case 'F':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_no_fg_color)+1)*sizeof(char));
          strcat(*parsed_format, sgr_no_fg_color);
          i++;
          break;
        case 'b':
          i = ParseSgrModifierColors(format, parsed_format, i+1);
          break;
        case 'B':
          *parsed_format = realloc(*parsed_format, 
                                   (strlen(*parsed_format)+strlen(sgr_no_bg_color)+1)*sizeof(char));
          strcat(*parsed_format, sgr_no_bg_color);
          i++;
          break;
        default:
          break;
      }
    }
    i++;
  }
}


// TODO: It works but this feels pretty stupid, need to rethink/refactor
static unsigned long ParseSgrModifierColors(char *format, char **parsed_format, unsigned long i) {
  unsigned long j;

  // Forgeground color
  if(format[i] == 'f') {
    switch(format[i+1]) {
      case 'k':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_black)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_black);
        break;
      case 'K':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_gray)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_gray);
        break;
      case 'w':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_white)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_white);
        break;
      case 'W':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_silver)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_silver);
        break;
      case 'r':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_red)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_red);
        break;
      case 'R':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_bright_red)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_bright_red);
        break;
      case 'o':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_orange)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_orange);
        break;
      case 'O':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_bright_orange)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_bright_orange);
        break;
      case 'y':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_yellow)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_yellow);
        break;
      case 'Y':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_bright_yellow)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_bright_yellow);
        break;
      case 'g':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_green)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_green);
        break;
      case 'G':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_bright_green)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_bright_green);
        break;
      case 'c':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_cyan)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_cyan);
        break;
      case 'C':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_bright_cyan)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_bright_cyan);
        break;
      case 'b':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_blue)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_blue);
        break;
      case 'B':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_bright_blue)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_bright_blue);
        break;
      case 'm':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_magenta)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_magenta);
        break;
      case 'M':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_static_bright_magenta)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_static_bright_magenta);
        break;
      case '1':
        switch(format[i+2]) {
          case '0':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_fg_variable_bright_red)+1)*sizeof(char));
            strcat(*parsed_format, sgr_fg_variable_bright_red);
            break;
          case '1':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_fg_variable_bright_green)+1)*sizeof(char));
            strcat(*parsed_format, sgr_fg_variable_bright_green);
            break;
          case '2':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_fg_variable_bright_yellow)+1)*sizeof(char));
            strcat(*parsed_format, sgr_fg_variable_bright_yellow);
            break;
          case '3':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_fg_variable_bright_blue)+1)*sizeof(char));
            strcat(*parsed_format, sgr_fg_variable_bright_blue);
            break;
          case '4':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_fg_variable_bright_magenta)+1)*sizeof(char));
            strcat(*parsed_format, sgr_fg_variable_bright_magenta);
            break;
          case '5':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_fg_variable_bright_cyan)+1)*sizeof(char));
            strcat(*parsed_format, sgr_fg_variable_bright_cyan);
            break;
          case '6':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_fg_variable_bright_white)+1)*sizeof(char));
            strcat(*parsed_format, sgr_fg_variable_bright_white);
            break;
          default:
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_fg_variable_black)+1)*sizeof(char));
            strcat(*parsed_format, sgr_fg_variable_black);
            i--;
            break;
        }
        i++;
        break;
      case '2':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_variable_red)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_variable_red);
        break;
      case '3':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_variable_green)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_variable_green);
        break;
      case '4':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_variable_yellow)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_variable_yellow);
        break;
      case '5':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_variable_blue)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_variable_blue);
        break;
      case '6':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_variable_magenta)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_variable_magenta);
        break;
      case '7':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_variable_cyan)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_variable_cyan);
        break;
      case '8':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_variable_white)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_variable_white);
        break;
      case '9':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_variable_bright_black)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_variable_bright_black);
        break;
      case '(':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_fg_rgb_begin)+1)*sizeof(char));
        strcat(*parsed_format, sgr_fg_rgb_begin);
        i += 2;
        j = i;
        while(format[i] != '\0') {
          if(format[i] == ',') {
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_rgb_a)+i-j+1)*sizeof(char));
            strncat(*parsed_format, format+j, i-j);
            strcat(*parsed_format, sgr_rgb_a);
            j = i + 1;
          }
          else if(format[i] == ')') {
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_rgb_m)+i-j+1)*sizeof(char));
            strncat(*parsed_format, format+j, i-j);
            strcat(*parsed_format, sgr_rgb_m);
            i--;
            break;
          }
          i++;
        }
        break;
    }
  }
  // Background color
  else {
    switch(format[i+1]) {
      case 'k':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_black)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_black);
        break;
      case 'K':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_gray)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_gray);
        break;
      case 'w':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_white)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_white);
        break;
      case 'W':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_silver)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_silver);
        break;
      case 'r':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_red)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_red);
        break;
      case 'R':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_bright_red)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_bright_red);
        break;
      case 'o':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_orange)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_orange);
        break;
      case 'O':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_bright_orange)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_bright_orange);
        break;
      case 'y':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_yellow)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_yellow);
        break;
      case 'Y':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_bright_yellow)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_bright_yellow);
        break;
      case 'g':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_green)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_green);
        break;
      case 'G':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_bright_green)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_bright_green);
        break;
      case 'c':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_cyan)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_cyan);
        break;
      case 'C':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_bright_cyan)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_bright_cyan);
        break;
      case 'b':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_blue)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_blue);
        break;
      case 'B':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_bright_blue)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_bright_blue);
        break;
      case 'm':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_magenta)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_magenta);
        break;
      case 'M':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_static_bright_magenta)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_static_bright_magenta);
        break;
      case '1':
        switch(format[i+2]) {
          case '0':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_bg_variable_bright_red)+1)*sizeof(char));
            strcat(*parsed_format, sgr_bg_variable_bright_red);
            break;
          case '1':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_bg_variable_bright_green)+1)*sizeof(char));
            strcat(*parsed_format, sgr_bg_variable_bright_green);
            break;
          case '2':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_bg_variable_bright_yellow)+1)*sizeof(char));
            strcat(*parsed_format, sgr_bg_variable_bright_yellow);
            break;
          case '3':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_bg_variable_bright_blue)+1)*sizeof(char));
            strcat(*parsed_format, sgr_bg_variable_bright_blue);
            break;
          case '4':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_bg_variable_bright_magenta)+1)*sizeof(char));
            strcat(*parsed_format, sgr_bg_variable_bright_magenta);
            break;
          case '5':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_bg_variable_bright_cyan)+1)*sizeof(char));
            strcat(*parsed_format, sgr_bg_variable_bright_cyan);
            break;
          case '6':
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_bg_variable_bright_white)+1)*sizeof(char));
            strcat(*parsed_format, sgr_bg_variable_bright_white);
            break;
          default:
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_bg_variable_black)+1)*sizeof(char));
            strcat(*parsed_format, sgr_bg_variable_black);
            break;
        }
        break;
      case '2':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_variable_red)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_variable_red);
        break;
      case '3':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_variable_green)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_variable_green);
        break;
      case '4':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_variable_yellow)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_variable_yellow);
        break;
      case '5':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_variable_blue)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_variable_blue);
        break;
      case '6':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_variable_magenta)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_variable_magenta);
        break;
      case '7':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_variable_cyan)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_variable_cyan);
        break;
      case '8':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_variable_white)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_variable_white);
        break;
      case '9':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_variable_bright_black)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_variable_bright_black);
        break;
      case '(':
        *parsed_format = realloc(*parsed_format, 
                                 (strlen(*parsed_format)+strlen(sgr_bg_rgb_begin)+1)*sizeof(char));
        strcat(*parsed_format, sgr_bg_rgb_begin);
        i += 2;
        j = i;
        while(format[i] != '\0') {
          if(format[i] == ',') {
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_rgb_a)+i-j+1)*sizeof(char));
            strncat(*parsed_format, format+j, i-j);
            strcat(*parsed_format, sgr_rgb_a);
            j = i + 1;
          }
          else if(format[i] == ')') {
            *parsed_format = realloc(*parsed_format, 
                                     (strlen(*parsed_format)+strlen(sgr_rgb_m)+i-j+1)*sizeof(char));
            strncat(*parsed_format, format+j, i-j);
            strcat(*parsed_format, sgr_rgb_m);
            break;
          }
          i++;
        }
        break;
    }
  }

  i++;
  return i;
}
