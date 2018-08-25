#include "clog.h"

static ClLogProps logProps;
static FILE *fp;
static int currRolloverAppend;
static unsigned long currLogLen;
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
  logProps.level = CL_LOG_LEVEL_INFO;
  logProps.stream = CL_STREAM_ALL;
  logProps.color = CL_COLOR_ON;
  logProps.rollover = 8388608;
  logProps.filename = NULL;
  fp = NULL;
  currRolloverAppend = 1;
  currLogLen = 0;
}

void ClLoadLogProps(ClLogProps props) {
  if(fp != NULL) {
    fclose(fp);
  }
  logProps.level = props.level;
  logProps.stream = props.stream;
  logProps.color = props.color;
  logProps.rollover = props.rollover;
  logProps.filename = props.filename;
  //TODO: error: handling
  fp = fopen(logProps.filename, "a");
  currRolloverAppend = 1;
  currLogLen = 0;
}

void ClResetLogProps() {
  if(fp != NULL) {
    fclose(fp);
  }
  ClInitLogProps();
}

void ClSetPropLevel(ClLogLevels level) {
  logProps.level = level;
}

ClLogLevels ClGetPropLevel() {
  return logProps.level;
}

void ClSetPropStream(ClStreams stream) {
  logProps.stream = stream;
}

ClStreams ClGetPropStream() {
  return logProps.stream;
}

void ClSetPropColor(ClColor color) {
  logProps.color = color;
}

ClColor ClGetPropColor() {
  return logProps.color;
}

void ClSetPropRollover(long rollover) {
  if(rollover < 1024) {
    logProps.rollover = 1024;
  }
  else if(rollover > LONG_MAX) {
    logProps.rollover = LONG_MAX;
  }
  else {
    logProps.rollover = rollover;
  }
}

long ClGetPropRollover() {
  return logProps.rollover;
}

void ClSetPropFilename(char *filename) {
  logProps.filename = filename;
}

char *ClGetPropFilename() {
  return logProps.filename;
}

void ClLog(ClMsgLevels level, const char *srcFilename, int srcLine, const char *message, ...) {
  // Only consider logging if the message's level is within the global level bound
  if(logProps.level >= (ClLogLevels)level) {
    // Only print to the console if enabled
    if(logProps.stream == CL_STREAM_ALL || logProps.stream == CL_STREAM_CONSOLE) {
      FILE *cp;
      va_list args;

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
                colorReset, colorBlack, srcFilename, srcLine, colorReset);
      }
      else {
        fprintf(cp, simpleFormat, __DATE__, __TIME__, levels[level], srcFilename, srcLine);
      }

      // Print the logging message along with any option parameters
      va_start(args, message);
      vfprintf(cp, message, args);
      va_end(args);
      fprintf(cp, "\n");
      fflush(cp);
    }

    // Only print to the file pointer if one was provided
    if(logProps.stream == CL_STREAM_ALL || logProps.stream == CL_STREAM_DISK) {
      va_list args;

      // Make sure a valid filename was actually provided
      if(logProps.filename != NULL) {
        // Open the file pointer if it wasn't open already
        if(fp == NULL) {
          fp = fopen(logProps.filename, "a");
          currLogLen = ftell(fp);
        }

        // Print the logging message along with any option parameters
        currLogLen += fprintf(fp, simpleFormat, __DATE__, __TIME__, levels[level], srcFilename, 
                              srcLine);
        va_start(args, message);
        currLogLen += vfprintf(fp, message, args);
        va_end(args);
        currLogLen += fprintf(fp, "\n");
        fflush(fp);

        // Perform log rollover if necessary
        if(logProps.rollover < currLogLen) {
          int len = (int)strlen(logProps.filename);
          char rolledFilename[len+3+(int)floor(log10(currRolloverAppend))];
          FILE *tp;

          while(1) {
            //TODO: edge case where restarting program after 10 or more rolls have occurred, buffer size
            sprintf(rolledFilename, "%s.%d", logProps.filename, currRolloverAppend);
            tp = fopen(rolledFilename, "r");
            if(tp != NULL) {
              fclose(tp);
              currRolloverAppend++;
            }
            else {
              break;
            }
          }
          fclose(fp);
          rename(logProps.filename, rolledFilename);
          fp = fopen(logProps.filename, "a");
          currRolloverAppend++;
          currLogLen = 0;
        }
      }
    }
  }
}