#include "clog.h"


int main(int argc, char *argv[]) {
  ClInitLogProps();
  ClSetPropLevel(CL_LOG_LEVEL_ALL);

  LOG_FATAL("This is a fatal message");
  LOG_ERROR("This is an error message");
  LOG_WARN("This is a warning message");
  LOG_INFO("This is an information message");
  LOG_DEBUG("This is a debugging message");
  LOG_TRACE("This is a tracing message");

  return 0;
}