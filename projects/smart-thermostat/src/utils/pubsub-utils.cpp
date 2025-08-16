#include "utils/pubsub-utils.h"

namespace tstat {
namespace pubsub_utils {

// "Soft" singleton global object defined here,
//  but defined as extern and initialized in pubsub-utils.h.
PubSub pubSub;

void PubSub::publish(AnyRotaryHoldEvent* pEvent) {
  for (auto& callback : _anyRotaryHoldSubCallbacks) {
    callback(pEvent);
  }
}

void PubSub::publish(AllRotariesHoldEvent* pEvent) {
  for (auto& callback : _allRotariesHoldSubCallbacks) {
    callback(pEvent);
  }
}

void PubSub::publish(TargetTRotaryPressEvent* pEvent) {
  for (auto& callback : _targetTRotaryPressSubCallbacks) {
    callback(pEvent);
  }
}

void PubSub::publish(TargetTRotaryRotationEvent* pEvent) {
  for (auto& callback : _targetTRotaryChangeSubCallbacks) {
    callback(pEvent);
  }
}

void PubSub::publish(TimerRotaryPressEvent* pEvent) {
  for (auto& callback : _timerRotaryPressSubCallbacks) {
    callback(pEvent);
  }
}

void PubSub::publish(TimerRotaryRotationEvent* pEvent) {
  for (auto& callback : _timerRotaryChangeSubCallbacks) {
    callback(pEvent);
  }
}

void PubSub::publish(HeatingStatusUpdateEvent* pEvent) {
  for (auto& callback : _heatingStatusChangeSubCallbacks) {
    callback(pEvent);
  }
}

void PubSub::publish(ErrorStatusUpdateEvent* pEvent) {
  for (auto& callback : _errorStatusChangeSubCallbacks) {
    callback(pEvent);
  }
}

void PubSub::publish(SchedulerStartEvent* pEvent) {
  for (auto& callback : _newScheduleSubCallbacks) {
    callback(pEvent);
  }
}

void PubSub::publish(SchedulerTimerUpdateEvent* pEvent) {
  for (auto& callback : _schedulerEditTimeSubCallbacks) {
    callback(pEvent);
  }
}

void PubSub::publish(SchedulerTargetTUpdateEvent* pEvent) {
  for (auto& callback : _schedulerEditTargetTSubCallbacks) {
    callback(pEvent);
  }
}

void PubSub::publish(SchedulerEndEvent* pEvent) {
  for (auto& callback : _schedulerOverSubCallbacks) {
    callback(pEvent);
  }
}

void PubSub::subscribe(std::function<void(AnyRotaryHoldEvent*)> callback) {
  _anyRotaryHoldSubCallbacks.push_back(callback);
}

void PubSub::subscribe(std::function<void(AllRotariesHoldEvent*)> callback) {
  _allRotariesHoldSubCallbacks.push_back(callback);
}

void PubSub::subscribe(std::function<void(TargetTRotaryPressEvent*)> callback) {
  _targetTRotaryPressSubCallbacks.push_back(callback);
}

void PubSub::subscribe(
    std::function<void(TargetTRotaryRotationEvent*)> callback) {
  _targetTRotaryChangeSubCallbacks.push_back(callback);
}

void PubSub::subscribe(std::function<void(TimerRotaryPressEvent*)> callback) {
  _timerRotaryPressSubCallbacks.push_back(callback);
}

void PubSub::subscribe(
    std::function<void(TimerRotaryRotationEvent*)> callback) {
  _timerRotaryChangeSubCallbacks.push_back(callback);
}

void PubSub::subscribe(
    std::function<void(HeatingStatusUpdateEvent*)> callback) {
  _heatingStatusChangeSubCallbacks.push_back(callback);
}

void PubSub::subscribe(std::function<void(ErrorStatusUpdateEvent*)> callback) {
  _errorStatusChangeSubCallbacks.push_back(callback);
}

void PubSub::subscribe(std::function<void(SchedulerStartEvent*)> callback) {
  _newScheduleSubCallbacks.push_back(callback);
}

void PubSub::subscribe(
    std::function<void(SchedulerTimerUpdateEvent*)> callback) {
  _schedulerEditTimeSubCallbacks.push_back(callback);
}

void PubSub::subscribe(
    std::function<void(SchedulerTargetTUpdateEvent*)> callback) {
  _schedulerEditTargetTSubCallbacks.push_back(callback);
}

void PubSub::subscribe(std::function<void(SchedulerEndEvent*)> callback) {
  _schedulerOverSubCallbacks.push_back(callback);
}

}  // namespace pubsub_utils
}  // namespace tstat
