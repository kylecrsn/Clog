#include "clog.h"

static ClLogProps logProps;
static ClFmtChunk *format;
static int formatLen;
static FILE *fp;
static int currRolloverAppend;
static unsigned long currLogLen;
static char *defaultFormat = "%t{%Y-%m-%d} %t{%H:%M:%S} %l %cK(%F:%L)%r: %m";
static char *txtReset = "\x1b[0m";
static char *txtColors[] = {
  "\x1b[30m", "\x1b[31m", "\x1b[32m", "\x1b[33m", "\x1b[34m", "\x1b[35m", "\x1b[36m", "\x1b[37m",
  "\x1b[90m", "\x1b[91m", "\x1b[92m", "\x1b[93m", "\x1b[94m", "\x1b[95m", "\x1b[96m", "\x1b[97m"
};
static const int defaultColors[] = {
  13, 9, 11, 10, 12, 14
};
static const char *levels[] = {
  "FATAL", "ERROR", "WARN ", "INFO ", "DEBUG", "TRACE"
};

static unsigned long 
PrintFormat(FILE *p, ClStreams stream, ClMsgLevels level, const char *filename, int line, 
            const char *function, const char *message, va_list args);
static void 
ParseFormat(char *fmt);
static void 
CaptureStr(char *fmt, int i, int j);
static int 
ColorCharToIndex(char c);

void ClInitLogProps() {
  logProps.level = CL_LOG_LEVEL_INFO;
  logProps.stream = CL_STREAM_ALL;
  logProps.color = CL_COLOR_ON;
  logProps.rollover = 1048576;
  logProps.filename = NULL;
  logProps.format = defaultFormat;
  fp = NULL;
  currRolloverAppend = 1;
  currLogLen = 0;
  ParseFormat(logProps.format);
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
  logProps.format = props.format;
  //TODO: error: handling
  fp = fopen(logProps.filename, "a");
  currRolloverAppend = 1;
  currLogLen = 0;
  ParseFormat(logProps.format);
}

void ClResetLogProps() {
  if(fp != NULL) {
    fclose(fp);
  }
  ClInitLogProps();
}

void ClCleanupLogProps() {
  if(fp != NULL) {
    fclose(fp);
  }
  if(formatLen > 0) {
    int i;
    for(i = 0; i < formatLen; i++) {
      if(format[i].type == CL_FMT_TYPE_TIME && format[i].str != NULL) {
        free(format[i].str);
      }
    }
    free(format);
  }
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

void 
ClLog(ClMsgLevels level, const char *filename, int line, 
      const char *function, const char *message, ...) {
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

      va_start(args, message);
      PrintFormat(cp, CL_STREAM_CONSOLE, level, filename, line, function, message, args);
      va_end(args);
    }

    // Only print to the disk if enabled
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
        va_start(args, message);
        currLogLen += PrintFormat(fp, CL_STREAM_DISK, level, filename, line, function, message, args);
        va_end(args);

        // Perform log rollover if necessary
        if(logProps.rollover < currLogLen) {
          int len = (int)strlen(logProps.filename);
          FILE *tp;

          while(1) {
            char rolledFilename[len+3+(int)floor(log10(currRolloverAppend))];

            sprintf(rolledFilename, "%s.%d", logProps.filename, currRolloverAppend);
            tp = fopen(rolledFilename, "r");
            if(tp != NULL) {
              fclose(tp);
              currRolloverAppend++;
            }
            else {
              fclose(fp);
              rename(logProps.filename, rolledFilename);
              fp = fopen(logProps.filename, "a");
              currRolloverAppend++;
              currLogLen = 0;
              break;
            }
          }
        }
      }
    }
  }
}

static unsigned long 
PrintFormat(FILE *p, ClStreams stream, ClMsgLevels level, const char *filename, int line, 
            const char *function, const char *message, va_list args) {
  int i;
  int tmLen;
  int tmMax = 256;
  unsigned long len = 0;
  char *tm;
  time_t rawtime;

  for(i = 0; i < formatLen; i++) {
    switch(format[i].type) {
      case CL_FMT_TYPE_STRING:
        len += fprintf(p, "%s", format[i].str);
        break;
      case CL_FMT_TYPE_TEXT_COLOR:
      case CL_FMT_TYPE_TEXT_RESET:
        if(logProps.color == CL_COLOR_ON && stream == CL_STREAM_CONSOLE) {
          len += fprintf(p, "%s", format[i].str);
        }
        break;
      case CL_FMT_TYPE_TIME:
        tm = malloc(tmMax*sizeof(char));
        time(&rawtime);
        tmLen = strftime(tm, tmMax, format[i].str, localtime(&rawtime));
        if(tmLen == 0) {
          tm[tmMax-1] = '\0';
        }
        else {
          tm = realloc(tm, (tmLen+1)*sizeof(char));
        }
        len += fprintf(p, "%s", tm);
        free(tm);
        break;
      case CL_FMT_TYPE_LEVEL:
        if(logProps.color == CL_COLOR_ON && stream == CL_STREAM_CONSOLE) {
          len += fprintf(p, "%s%s%s", txtColors[defaultColors[level]], levels[level], txtReset);
        }
        else {
          len += fprintf(p, "%s", levels[level]);
        }
        break;
      case CL_FMT_TYPE_FILENAME:
        len += fprintf(p, "%s", filename);
        break;
      case CL_FMT_TYPE_LINE:
        len += fprintf(p, "%d", line);
        break;
      case CL_FMT_TYPE_FUNCTION:
        len += fprintf(p, "%s", function);
        break;
      case CL_FMT_TYPE_PROCESS_ID:
        len += fprintf(p, "%d", (int)getpid());
        break;
      case CL_FMT_TYPE_MESSAGE:
        len += vfprintf(p, message, args);
        break;
      default:
        break;
    }
  }

  len += fprintf(p, "\n");
  fflush(p);
  return len;
}

static void 
ParseFormat(char *fmt) {
  int i;
  int j;
  int colorIdx;
  int len = strlen(fmt);
  char c;

  formatLen = 0;
  for(i = 0, j = 0; i < len; i++) {
    if(fmt[i] == '%') {
      switch(fmt[i+1]) {
        case 'c':
          colorIdx = ColorCharToIndex(fmt[i+2]);
          if(colorIdx != -1) {
            CaptureStr(fmt, i, j);
            format[formatLen].type = CL_FMT_TYPE_TEXT_COLOR;
            format[formatLen].str = txtColors[colorIdx];
            formatLen++;
            i += 2;
            j = i + 1;
          }
          break;
        case 'r':
          CaptureStr(fmt, i, j);
          format[formatLen].type = CL_FMT_TYPE_TEXT_RESET;
          format[formatLen].str = txtReset;
          formatLen++;
          i++;
          j = i + 1;
          break;
        case 't':
          if(fmt[i+2] == '{') {
            CaptureStr(fmt, i, j);
            format[formatLen].type = CL_FMT_TYPE_TIME;
            i += 2;
            j = i + 1;
            do {
              i++;
              c = fmt[i];
            } while(c != '}' && c != '\0');
            if(i-j != 0) {
              format[formatLen].str = malloc((i-j+1)*sizeof(char));
              strncpy(format[formatLen].str, fmt+j, i-j);
              format[formatLen].str[i-j] = '\0';
              formatLen++;
              j = i + 1;
            }
          }
          break;
        case 'l':
          CaptureStr(fmt, i, j);
          format[formatLen].type = CL_FMT_TYPE_LEVEL;
          formatLen++;
          i++;
          j = i + 1;
          break;
        case 'F':
          CaptureStr(fmt, i, j);
          format[formatLen].type = CL_FMT_TYPE_FILENAME;
          formatLen++;
          i++;
          j = i + 1;
          break;
        case 'L':
          CaptureStr(fmt, i, j);
          format[formatLen].type = CL_FMT_TYPE_LINE;
          formatLen++;
          i++;
          j = i + 1;
          break;
        case 'f':
          CaptureStr(fmt, i, j);
          format[formatLen].type = CL_FMT_TYPE_FUNCTION;
          formatLen++;
          i++;
          j = i + 1;
          break;
        case 'p':
          CaptureStr(fmt, i, j);
          format[formatLen].type = CL_FMT_TYPE_PROCESS_ID;
          formatLen++;
          i++;
          j = i + 1;
          break;
        case 'm':
          CaptureStr(fmt, i, j);
          format[formatLen].type = CL_FMT_TYPE_MESSAGE;
          formatLen++;
          i++;
          j = i + 1;
          break;
        default:
          break;
      }
    }
  }
}

static void 
CaptureStr(char *fmt, int i, int j) {
  if(i-j > 0) {
    format = (formatLen == 0) ? 
              malloc(2*sizeof(ClFmtChunk)) : 
              realloc(format, (formatLen+2)*sizeof(ClFmtChunk));
    format[formatLen].type = CL_FMT_TYPE_STRING;
    format[formatLen].str = malloc((i-j+1)*sizeof(char));
    strncpy(format[formatLen].str, fmt+j, i-j);
    format[formatLen].str[i-j] = '\0';
    formatLen++;
  }
  else {
    format = (formatLen == 0) ? 
              malloc(sizeof(ClFmtChunk)) : 
              realloc(format, (formatLen+1)*sizeof(ClFmtChunk));
  }
}

static int 
ColorCharToIndex(char c) {
  switch(c) {
    case 'k':
      return 0;
    case 'r':
      return 1;
    case 'g':
      return 2;
    case 'y':
      return 3;
    case 'b':
      return 4;
    case 'm':
      return 5;
    case 'c':
      return 6;
    case 'w':
      return 7;
    case 'K':
      return 8;
    case 'R':
      return 9;
    case 'G':
      return 10;
    case 'Y':
      return 11;
    case 'B':
      return 12;
    case 'M':
      return 13;
    case 'C':
      return 14;
    case 'W':
      return 15;
    default:
      return -1;
  }
}