#include "clog.h"

int main(int argc, char **argv) {
  ClInit();

  LOG_INFO("Test info message");

  LOG_FATAL("This is a %s", "fatal message");

  ClCleanup();

  return 0;
}
