#include "clog.h"

static ClLogProps    gLogProps;
static ClFmtChunk *  gFmt;
static int           gFmtLen;
static FILE *        gFp;
static int           gCurrRolloverAppend;
static unsigned long gCurrLogLen;
static char *        gDefaultFmt = "%t{%Y-%m-%d} %t{%H:%M:%S} %l %cK(%F:%L)%r: %m";
static char *        gTxtReset = "\x1b[0m";
static char *        gTxtColors[] = {
  "\x1b[30m", "\x1b[31m", "\x1b[32m", "\x1b[33m", "\x1b[34m", "\x1b[35m", "\x1b[36m", "\x1b[37m",
  "\x1b[90m", "\x1b[91m", "\x1b[92m", "\x1b[93m", "\x1b[94m", "\x1b[95m", "\x1b[96m", "\x1b[97m"
};
static const int     gDefaultColors[] = {
  13, 9, 11, 10, 12, 14
};
static const char *  gLevels[] = {
  "FATAL", "ERROR", "WARN ", "INFO ", "DEBUG", "TRACE"
};

static unsigned long 
PrintFormat(FILE *p, ClStreams stream, ClMsgLevels level, const char *filename, int line, 
            const char *function, const char *message, va_list args);
static void 
ParseFormat(char *format);
static void 
CaptureStr(char *format, int i, int j);
static int 
ColorCharToIndex(char c);

void ClInitLogProps() {
  gLogProps.level = CL_LOG_LEVEL_INFO;
  gLogProps.stream = CL_STREAM_ALL;
  gLogProps.color = CL_COLOR_ON;
  gLogProps.rollover = 1048576;
  gLogProps.filename = NULL;
  gLogProps.format = gDefaultFmt;
  gFp = NULL;
  gCurrRolloverAppend = 1;
  gCurrLogLen = 0;
  ParseFormat(gLogProps.format);
}

void ClCleanupLogProps() {
  if(gFp != NULL) {
    fclose(gFp);
    gFp = NULL;
  }
  if(gFmtLen > 0) {
    int i;
    for(i = 0; i < gFmtLen; i++) {
      if((gFmt[i].type == CL_FMT_TYPE_TIME || 
          gFmt[i].type == CL_FMT_TYPE_STRING) && 
          gFmt[i].str != NULL) {
        free(gFmt[i].str);
      }
    }
    free(gFmt);
  }
}

void ClResetLogProps() {
  ClCleanupLogProps();
  ClInitLogProps();
}

void ClLoadLogProps(ClLogProps props) {
  ClCleanupLogProps();
  gLogProps.level = props.level;
  gLogProps.stream = props.stream;
  gLogProps.color = props.color;
  gLogProps.rollover = props.rollover;
  gLogProps.filename = props.filename;
  gLogProps.format = props.format;
  gFp = fopen(gLogProps.filename, "a");
  gCurrRolloverAppend = 1;
  gCurrLogLen = 0;
  ParseFormat(gLogProps.format);
}

void ClSetPropLevel(ClLogLevels level) {
  gLogProps.level = level;
}

ClLogLevels ClGetPropLevel() {
  return gLogProps.level;
}

void ClSetPropStream(ClStreams stream) {
  gLogProps.stream = stream;
}

ClStreams ClGetPropStream() {
  return gLogProps.stream;
}

void ClSetPropColor(ClColor color) {
  gLogProps.color = color;
}

ClColor ClGetPropColor() {
  return gLogProps.color;
}

void ClSetPropRollover(long rollover) {
  if(rollover < 1024) {
    gLogProps.rollover = 1024;
  }
  else if(rollover > LONG_MAX) {
    gLogProps.rollover = LONG_MAX;
  }
  else {
    gLogProps.rollover = rollover;
  }
}

long ClGetPropRollover() {
  return gLogProps.rollover;
}

void ClSetPropFilename(char *filename) {
  gLogProps.filename = filename;
}

char *ClGetPropFilename() {
  return gLogProps.filename;
}

void ClSetPropFormat(char *format) {
  gLogProps.format = format;
  ParseFormat(gLogProps.format);
}

char *ClGetPropFormat() {
  return gLogProps.format;
}

void 
ClLog(ClMsgLevels level, const char *filename, int line, 
      const char *function, const char *message, ...) {
  // Only consider logging if the message's level is within the global level bound
  if(gLogProps.level >= (ClLogLevels)level) {
    // Only print to the console if enabled
    if(gLogProps.stream == CL_STREAM_ALL || gLogProps.stream == CL_STREAM_CONSOLE) {
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
    if(gLogProps.stream == CL_STREAM_ALL || gLogProps.stream == CL_STREAM_DISK) {
      va_list args;

      // Make sure a valid filename was actually provided
      if(gLogProps.filename != NULL) {
        // Open the file pointer if it wasn't open already
        if(gFp == NULL) {
          gFp = fopen(gLogProps.filename, "a");
          gCurrLogLen = ftell(gFp);
        }

        // Print the logging message along with any option parameters
        va_start(args, message);
        gCurrLogLen += PrintFormat(gFp, CL_STREAM_DISK, level, filename, line, function, message, args);
        va_end(args);

        // If no data was ever written (in the case of an empty format string), close and remove 
        // the file.
        if(gCurrLogLen == 0) {
          fclose(gFp);
          gFp = NULL;
          remove(gLogProps.filename);
        }

        // Perform log rollover if necessary
        if(gLogProps.rollover < gCurrLogLen) {
          int len = (int)strlen(gLogProps.filename);
          FILE *tp;

          while(1) {
            char rolledFilename[len+3+(int)floor(log10(gCurrRolloverAppend))];

            sprintf(rolledFilename, "%s.%d", gLogProps.filename, gCurrRolloverAppend);
            tp = fopen(rolledFilename, "r");
            if(tp != NULL) {
              fclose(tp);
              tp = NULL;
              gCurrRolloverAppend++;
            }
            else {
              fclose(gFp);
              gFp = NULL;
              rename(gLogProps.filename, rolledFilename);
              gFp = fopen(gLogProps.filename, "a");
              gCurrRolloverAppend++;
              gCurrLogLen = 0;
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
  unsigned long len = 0;

  // Only print if the custom format is non-empty
  if(gFmtLen > 0) {
    int i;
    int tmLen;
    int tmMax = 256;
    char *tm;
    time_t rawtime;

    for(i = 0; i < gFmtLen; i++) {
      switch(gFmt[i].type) {
        case CL_FMT_TYPE_STRING:
          len += fprintf(p, "%s", gFmt[i].str);
          break;
        case CL_FMT_TYPE_TEXT_COLOR:
        case CL_FMT_TYPE_TEXT_RESET:
          if(gLogProps.color == CL_COLOR_ON && stream == CL_STREAM_CONSOLE) {
            len += fprintf(p, "%s", gFmt[i].str);
          }
          break;
        case CL_FMT_TYPE_TIME:
          tm = malloc(tmMax*sizeof(char));
          time(&rawtime);
          tmLen = strftime(tm, tmMax, gFmt[i].str, localtime(&rawtime));
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
          if(gLogProps.color == CL_COLOR_ON && stream == CL_STREAM_CONSOLE) {
            len += fprintf(p, "%s%s%s", gTxtColors[gDefaultColors[level]], gLevels[level], gTxtReset);
          }
          else {
            len += fprintf(p, "%s", gLevels[level]);
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
  }

  return len;
}

static void 
ParseFormat(char *format) {
  int i;
  int j;
  int colorIdx;
  int len = strlen(format);
  char c;

  gFmtLen = 0;
  for(i = 0, j = 0; i < len; i++) {
    if(format[i] == '%') {
      switch(format[i+1]) {
        case 'c':
          colorIdx = ColorCharToIndex(format[i+2]);
          if(colorIdx != -1) {
            CaptureStr(format, i, j);
            gFmt[gFmtLen].type = CL_FMT_TYPE_TEXT_COLOR;
            gFmt[gFmtLen].str = gTxtColors[colorIdx];
            gFmtLen++;
            i += 2;
            j = i + 1;
          }
          else {
            i++;
          }
          break;
        case 'r':
          CaptureStr(format, i, j);
          gFmt[gFmtLen].type = CL_FMT_TYPE_TEXT_RESET;
          gFmt[gFmtLen].str = gTxtReset;
          gFmtLen++;
          i++;
          j = i + 1;
          break;
        case 't':
          if(format[i+2] == '{') {
            int oldI = i;
            int oldJ = j;
            i += 2;
            j = i + 1;
            do {
              i++;
              c = format[i];
            } while(c != '}' && c != '\0');
            if(c == '}') {
              CaptureStr(format, oldI, oldJ);
              gFmt[gFmtLen].type = CL_FMT_TYPE_TIME;
              gFmt[gFmtLen].str = malloc((i-j+1)*sizeof(char));
              strncpy(gFmt[gFmtLen].str, format+j, i-j);
              gFmt[gFmtLen].str[i-j] = '\0';
              gFmtLen++;
              j = i + 1;
            }
            else {
              i = oldI + 1;
              j = oldJ;
            }
          }
          else {
            i++;
          }
          break;
        case 'l':
          CaptureStr(format, i, j);
          gFmt[gFmtLen].type = CL_FMT_TYPE_LEVEL;
          gFmtLen++;
          i++;
          j = i + 1;
          break;
        case 'F':
          CaptureStr(format, i, j);
          gFmt[gFmtLen].type = CL_FMT_TYPE_FILENAME;
          gFmtLen++;
          i++;
          j = i + 1;
          break;
        case 'L':
          CaptureStr(format, i, j);
          gFmt[gFmtLen].type = CL_FMT_TYPE_LINE;
          gFmtLen++;
          i++;
          j = i + 1;
          break;
        case 'f':
          CaptureStr(format, i, j);
          gFmt[gFmtLen].type = CL_FMT_TYPE_FUNCTION;
          gFmtLen++;
          i++;
          j = i + 1;
          break;
        case 'p':
          CaptureStr(format, i, j);
          gFmt[gFmtLen].type = CL_FMT_TYPE_PROCESS_ID;
          gFmtLen++;
          i++;
          j = i + 1;
          break;
        case 'm':
          CaptureStr(format, i, j);
          gFmt[gFmtLen].type = CL_FMT_TYPE_MESSAGE;
          gFmtLen++;
          i++;
          j = i + 1;
          break;
        default:
          break;
      }
    }
  }
  if(i-j > 0) {
    gFmt = (gFmtLen == 0) ? 
            malloc(sizeof(ClFmtChunk)) : 
            realloc(gFmt, (gFmtLen+1)*sizeof(ClFmtChunk));
    gFmt[gFmtLen].type = CL_FMT_TYPE_STRING;
    gFmt[gFmtLen].str = malloc((i-j+1)*sizeof(char));
    strncpy(gFmt[gFmtLen].str, format+j, i-j);
    gFmt[gFmtLen].str[i-j] = '\0';
    gFmtLen++;
  }
}

static void 
CaptureStr(char *format, int i, int j) {
  if(i-j > 0) {
    gFmt = (gFmtLen == 0) ? 
              malloc(2*sizeof(ClFmtChunk)) : 
              realloc(gFmt, (gFmtLen+2)*sizeof(ClFmtChunk));
    gFmt[gFmtLen].type = CL_FMT_TYPE_STRING;
    gFmt[gFmtLen].str = malloc((i-j+1)*sizeof(char));
    strncpy(gFmt[gFmtLen].str, format+j, i-j);
    gFmt[gFmtLen].str[i-j] = '\0';
    gFmtLen++;
  }
  else {
    gFmt = (gFmtLen == 0) ? 
              malloc(sizeof(ClFmtChunk)) : 
              realloc(gFmt, (gFmtLen+1)*sizeof(ClFmtChunk));
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