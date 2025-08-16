#ifndef _PUBSUB_UTILS_H
#define _PUBSUB_UTILS_H

#include <functional>
#include <list>

#include "utils/time-utils.h"


// TODO IMP: checkout the better version that does NOT use pointers:
//  https://github.com/puntonim/arduino-monorepo/tree/main/SW__PROJECT_TEMPLATES/3-big-project-w-display-and-rotary-template


namespace tstat {
namespace pubsub_utils {

class BasePubSubEvent {
 public:
  constexpr static char topic[] = "BASE_EVENT";

 private:
};

class AnyRotaryHoldEvent : public BasePubSubEvent {
 public:
  constexpr static char topic[] = "ANY_ROTARY_HOLD_EVENT";
};

class AllRotariesHoldEvent : public BasePubSubEvent {
 public:
  constexpr static char topic[] = "ALL_ROTARIES_HOLD_EVENT";
};

class TargetTRotaryPressEvent : public BasePubSubEvent {
 public:
  constexpr static char topic[] = "TARGET_T_ROTARY_PRESS_EVENT";
};

class TargetTRotaryRotationEvent : public BasePubSubEvent {
 public:
  constexpr static char topic[] = "TARGET_T_ROTARY_CHANGE_EVENT";
  const int16_t value;
  const bool isDisplayOn;
  TargetTRotaryRotationEvent(const int16_t value, const bool isDisplayOn)
      : value(value), isDisplayOn(isDisplayOn) {};
};

class TimerRotaryPressEvent : public BasePubSubEvent {
 public:
  constexpr static char topic[] = "TIMER_ROTARY_PRESS_EVENT";
};

class TimerRotaryRotationEvent : public BasePubSubEvent {
 public:
  constexpr static char topic[] = "TIMER_ROTARY_CHANGE_EVENT";
  const int16_t value;
  const bool isDisplayOn;
  TimerRotaryRotationEvent(const int16_t value, bool isDisplayOn)
      : value(value), isDisplayOn(isDisplayOn) {};
};

class HeatingStatusUpdateEvent : public BasePubSubEvent {
 public:
  constexpr static char topic[] = "HEATING_STATUS_CHANGE_EVENT";
  const bool isOn;
  HeatingStatusUpdateEvent(bool isOn) : isOn(isOn) {};
};

class ErrorStatusUpdateEvent : public BasePubSubEvent {
 public:
  constexpr static char topic[] = "ERROR_STATUS_CHANGE_EVENT";
  const bool isError;
  std::list<char*> messageList;
  ErrorStatusUpdateEvent(bool isError, std::list<char*> messageList = {})
      : isError(isError), messageList(messageList) {};
};

// TODO rename to TimerStartEvent. No need to add the new concept Scheduler.
class SchedulerStartEvent : public BasePubSubEvent {
 public:
  constexpr static char topic[] = "NEW_SCHEDULE_EVENT";
};

// TODO rename to TimerStoptEvent. No need to add the new concept Scheduler.
class SchedulerEndEvent : public BasePubSubEvent {
 public:
  constexpr static char topic[] = "SCHEDULER_OVER_EVENT";
};

// TODO rename to TimerUpdateEvent. No need to add the new concept Scheduler.
class SchedulerTimerUpdateEvent : public BasePubSubEvent {
 public:
  constexpr static char topic[] = "SCHEDULER_EDIT_TIME_EVENT";
  time_utils::Time time;
  SchedulerTimerUpdateEvent(time_utils::Time time) : time(time) {};
};

// TODO rename to TargetTUpdateEvent. No need to add the new concept Scheduler.
class SchedulerTargetTUpdateEvent : public BasePubSubEvent {
 public:
  constexpr static char topic[] = "SCHEDULER_EDIT_TARGET_T_EVENT";
  u_int8_t targetTemperature;
  SchedulerTargetTUpdateEvent(u_int8_t t) : targetTemperature(t) {};
};

class PubSub {
 private:
  std::list<std::function<void(AnyRotaryHoldEvent*)>>
      _anyRotaryHoldSubCallbacks;
  std::list<std::function<void(AllRotariesHoldEvent*)>>
      _allRotariesHoldSubCallbacks;
  std::list<std::function<void(TargetTRotaryPressEvent*)>>
      _targetTRotaryPressSubCallbacks;
  std::list<std::function<void(TargetTRotaryRotationEvent*)>>
      _targetTRotaryChangeSubCallbacks;
  std::list<std::function<void(TimerRotaryPressEvent*)>>
      _timerRotaryPressSubCallbacks;
  std::list<std::function<void(TimerRotaryRotationEvent*)>>
      _timerRotaryChangeSubCallbacks;
  std::list<std::function<void(HeatingStatusUpdateEvent*)>>
      _heatingStatusChangeSubCallbacks;
  std::list<std::function<void(ErrorStatusUpdateEvent*)>>
      _errorStatusChangeSubCallbacks;
  std::list<std::function<void(SchedulerStartEvent*)>> _newScheduleSubCallbacks;
  std::list<std::function<void(SchedulerTimerUpdateEvent*)>>
      _schedulerEditTimeSubCallbacks;
  std::list<std::function<void(SchedulerTargetTUpdateEvent*)>>
      _schedulerEditTargetTSubCallbacks;
  std::list<std::function<void(SchedulerEndEvent*)>> _schedulerOverSubCallbacks;

 public:
  void publish(AnyRotaryHoldEvent* pEvent);
  void publish(AllRotariesHoldEvent* pEvent);
  void publish(TargetTRotaryPressEvent* pEvent);
  void publish(TargetTRotaryRotationEvent* pEvent);
  void publish(TimerRotaryPressEvent* pEvent);
  void publish(TimerRotaryRotationEvent* pEvent);
  void publish(HeatingStatusUpdateEvent* pEvent);
  void publish(ErrorStatusUpdateEvent* pEvent);
  void publish(SchedulerStartEvent* pEvent);
  void publish(SchedulerTimerUpdateEvent* pEvent);
  void publish(SchedulerTargetTUpdateEvent* pEvent);
  void publish(SchedulerEndEvent* pEvent);

  void subscribe(std::function<void(AnyRotaryHoldEvent*)> callback);
  void subscribe(std::function<void(AllRotariesHoldEvent*)> callback);
  void subscribe(std::function<void(TargetTRotaryPressEvent*)> callback);
  void subscribe(std::function<void(TargetTRotaryRotationEvent*)> callback);
  void subscribe(std::function<void(TimerRotaryPressEvent*)> callback);
  void subscribe(std::function<void(TimerRotaryRotationEvent*)> callback);
  void subscribe(std::function<void(HeatingStatusUpdateEvent*)> callback);
  void subscribe(std::function<void(ErrorStatusUpdateEvent*)> callback);
  void subscribe(std::function<void(SchedulerStartEvent*)> callback);
  void subscribe(std::function<void(SchedulerTimerUpdateEvent*)> callback);
  void subscribe(std::function<void(SchedulerTargetTUpdateEvent*)> callback);
  void subscribe(std::function<void(SchedulerEndEvent*)> callback);
};

// "Soft" singleton global object defined as extern and initialized here,
//  but also defined in pubsub-utils.cpp.
extern PubSub pubSub;

}  // namespace pubsub_utils
}  // namespace tstat
#endif
