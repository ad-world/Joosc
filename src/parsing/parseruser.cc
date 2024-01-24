#include <iostream>
#include "bison/driver.h"
#include "parseuser.h"

int parsemain(int argc, char *argv[]) {
  int res = 0;
  Driver driver;
  driver.trace_parsing = true;
  driver.trace_scanning = true;

  for (int i = 1; i < argc; ++i) {
    if (!driver.parse(argv[i]))
      std::cout << driver.result << '\n';
    else
      res = 1;
  }

  return res;
}
