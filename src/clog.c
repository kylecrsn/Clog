#include "clog.h"

static ClLogProps logProps;
static const char *simpleFormat = "%s %s %-5s (%s:%d): ";
static const char *coloredFormat = "%s %s %s%-5s%s %s(%s:%d)%s: ";
static const char *colorBlack = "\x1b[90m";
static const char *colorReset = "\x1b[0m";
static const char *levels[] = {
  "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE"
};
static const char *colors[] = {
  "\x1b[95m", "\x1b[91m", "\x1b[93m", "\x1b[92m", "\x1b[94m", "\x1b[96m"
};


void ClInitLogProps() {
  logProps.fp = NULL;
  logProps.level = CL_LOG_LEVEL_INFO;
  logProps.console = CL_CONSOLE_ON;
  logProps.color = CL_COLOR_ON;
}

void ClLoadLogProps(ClLogProps props) {
  logProps.fp = props.fp;
  logProps.level = props.level;
  logProps.console = props.console;
  logProps.color = props.color;
}

void ClResetLogProps() {
  ClInitLogProps();
}

void ClSetPropLevel(ClLogLevels level) {
  logProps.level = level;
}

ClLogLevels ClGetPropLevel() {
  return logProps.level;
}

void ClSetPropConsole(ClConsole console) {
  logProps.console = console;
}

ClConsole ClGetPropConsole() {
  return logProps.console;
}

void ClSetPropColor(ClColor color) {
  logProps.color = color;
}

ClColor ClGetPropColor() {
  return logProps.color;
}

void ClSetPropFp(FILE *fp) {
  logProps.fp = fp;
}

FILE *ClGetPropFp() {
  return logProps.fp;
}

void ClLog(ClMsgLevels level, const char *filename, int line, const char *message, ...) {
  FILE *cp;
  va_list args;

  // Only consider logging if the message's level is within the global level bound
  if(logProps.level >= (ClLogLevels)level) {
    // Only print to the console if enabled
    if(logProps.console == CL_CONSOLE_ON) {
      // Print to either stdout or stderr depending on the level given for this message
      if(level == CL_MSG_LEVEL_ERROR || level == CL_MSG_LEVEL_FATAL) {
        cp = stderr;
      }
      else {
        cp = stdout;
      }

      // Only use colored console printing if enabled
      if(logProps.color == CL_COLOR_ON) {
        fprintf(cp, coloredFormat, __DATE__, __TIME__, colors[level], levels[level], 
                colorReset, colorBlack, filename, line, colorReset);
      }
      else {
        fprintf(cp, simpleFormat, __DATE__, __TIME__, levels[level], filename, line);
      }

      // Print the logging message along with any option parameters
      va_start(args, message);
      vfprintf(cp, message, args);
      va_end(args);
      fprintf(cp, "\n");
      fflush(cp);
    }

    // Only print to the file pointer if one was provided
    if(logProps.fp != NULL) {
      // Print the logging message along with any option parameters
      fprintf(logProps.fp, simpleFormat, __DATE__, __TIME__, levels[level], filename, line);
      va_start(args, message);
      vfprintf(logProps.fp, message, args);
      va_end(args);
      fprintf(logProps.fp, "\n");
      fflush(logProps.fp);
    }
  }
}