#ifndef _TIME_UTILS_H
#define _TIME_UTILS_H

namespace incub {
namespace time_utils {

struct Time {
  short h;
  short m;
  short s;
};

unsigned long timeToSecs(Time time);
unsigned long toSecs(unsigned short hour, unsigned short minute,
                     unsigned short second);
Time secsToTime(unsigned long secs);
void format(char* string, Time time);
bool isOver(Time time);

class Timer {
 private:
  Time _time = {0, 0, 0};
  unsigned long _lastTickTs;

 public:
  Time getTime();
  void start(unsigned short hour, unsigned short minute, unsigned short second);
  Time add(unsigned short hour, unsigned short minute, unsigned short second);
  Time add(unsigned short second);
  Time subtract(unsigned short hour, unsigned short minute,
                unsigned short second);
  Time subtract(unsigned short second);
  struct Time tick();
  void format(char* string);
  bool isOver();
  void reset();
};

}  // namespace time_utils
}  // namespace incub
#endif
