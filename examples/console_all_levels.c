#include "clog.h"


int main(int argc, char *argv[]) {
  // Initialize internal ClLogProps instance ot sensible defaults
  ClInitLogProps();

  LOG_INFO("This is a basic logging message, with colored console output");

  LOG_INFO("This message includes %d substitutions thanks to %s functions", 2, "variadic");

  LOG_FATAL("This is a fatal message");
  LOG_ERROR("This is an error message");
  LOG_WARN("This is a warning message");
  LOG_INFO("This is an information message");

  // The default level property value is CL_LOG_LEVEL_INFO, thus this won't print
  LOG_DEBUG("This is a debugging message (that never gets printed)");

  // Change the level property to print all levels of messages
  ClSetPropLevel(CL_LOG_LEVEL_ALL);
  LOG_DEBUG("This is a debugging message");
  LOG_TRACE("This is a tracing message");

  return 0;
}