#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>

class Timer
{

public:
  Timer(unsigned long millis);
  void setTimeout(unsigned long millis);
  bool check();
  bool singleShot();
  void reset();

private:
  unsigned long  start_millis, timeout_millis;
  bool shot;

};

#endif


