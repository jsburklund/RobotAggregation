#include <algorithm>
#include <iostream>
#include <functional>
#include <fstream>
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/logging/argos_log.h>
#include <loop_functions/segregation_loop_function.h>

#include "args.h"

int main(int argc, const char **argv) {
  std::ofstream dev_null("test");
  LOG.DisableColoredOutput();
  LOG << "1\n";
  LOG.Flush();
  LOG.GetStream().rdbuf(dev_null.rdbuf());
  LOG << "2\n";
  LOG.Flush();
}
