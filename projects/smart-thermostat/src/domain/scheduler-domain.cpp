/* SchedulerDomain is just a dumb cointainer class that includes:
 *  - the countdown timer
 *  - the target temperature.
 * It does NOT tick the countdown timer and doesn't check the tearget T against
 *  the sensor T.
 */
#include "domain/scheduler-domain.h"

#include "utils/pubsub-utils.h"
#include "utils/time-utils.h"

namespace tstat {

// "Soft" singleton global object defined here,
//  but defined as extern and initialized in scheduler-domain.h.
SchedulerDomain schedulerDomain;

void SchedulerDomain::setup() {
  // SUBSCRIPTION TimerRotaryRotationEvent -------------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::TimerRotaryRotationEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "SchedulerDomain - received event: " +
                       pEvent->topic);
#endif
        this->_onTimerRotaryRotation(pEvent);
      });

  // SUBSCRIPTION TargetTRotaryRotationEvent -----------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::TargetTRotaryRotationEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "SchedulerDomain - received event: " +
                       pEvent->topic);
#endif
        this->_onTargetTRotaryRotation(pEvent);
      });

  // SUBSCRIPTION AnyRotaryHoldEvent -------------------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::AnyRotaryHoldEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "SchedulerDomain - received event: " +
                       pEvent->topic);
#endif
        this->reset();
      });

  // SUBSCRIPTION AllRotariesHoldEvent -----------------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::AllRotariesHoldEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "SchedulerDomain - received event: " +
                       pEvent->topic);
#endif
        // This is a sort of hidden feature: when holding both rotary encoders,
        //  we set the countdown timer to 10 seconds. It is useful for a
        //  quick test.
        this->reset();
        timer.start(0, 0, 10);
        targetTemperature = settings::INITIAL_TARGET_T;
        pubsub_utils::pubSub.publish(new pubsub_utils::SchedulerStartEvent());
      });
}

bool SchedulerDomain::isScheduled() { return !timer.isOver(); }

/**
 * Handle the timer rotary encoder rotation.
 *
 * - if the display was OFF: noop (as we just have to switch on
 *    the display and this is done in display-device.cpp)
 * - if the display was ON and nothing was scheduled (SPENTO):
 *    - if the rotation was clockwise then start a new countdown timer with the
 *       initial timer setting
 *    - if the rotation was clockwise then noop
 * - if the display was ON and there was an ongoing schedule: just add or
 *    subtract time
 */
void SchedulerDomain::_onTimerRotaryRotation(
    pubsub_utils::TimerRotaryRotationEvent* pEvent) {
  // If the rotary encoder was rotated when the display was OFF, then noop (as
  //  as we just have to switch on the display and NOT to increment the timer).
  if (!pEvent->isDisplayOn) return;

  if (timer.isOver()) {
    if (pEvent->value > 0) {
      // If time is over and the timer rotary encoder was rotated clockwise,
      //  then start a new countdown timer with the initial time.
      timer.start(settings::INITIAL_TIMER.h, settings::INITIAL_TIMER.m,
                  settings::INITIAL_TIMER.s);
      targetTemperature = settings::INITIAL_TARGET_T;
      // And publish the new schedule event.
      pubsub_utils::pubSub.publish(new pubsub_utils::SchedulerStartEvent());
    }
  } else {
    // If time is not over, then add/remove the time.
    time_utils::Time time;
    if (pEvent->value > 0) {
      // Clockwise rotation: add time.
      time = timer.add(settings::DELTA_TIME_ON_ROTARY_ROTATION.h,
                       settings::DELTA_TIME_ON_ROTARY_ROTATION.m,
                       settings::DELTA_TIME_ON_ROTARY_ROTATION.s);
    } else {
      // Counterclockwise rotation: subtract time.
      time = timer.subtract(settings::DELTA_TIME_ON_ROTARY_ROTATION.h,
                            settings::DELTA_TIME_ON_ROTARY_ROTATION.m,
                            settings::DELTA_TIME_ON_ROTARY_ROTATION.s);
    }
    timer.tick();
    if (!timer.isOver()) {
      pubsub_utils::pubSub.publish(
          new pubsub_utils::SchedulerTimerUpdateEvent(time));
    } else {
      pubsub_utils::pubSub.publish(new pubsub_utils::SchedulerEndEvent());
    }
  }
}

/**
 * Handle the target T rotary encoder rotation.
 *
 * - if the display was OFF: noop (as we just have to switch on
 *    the display and this is done in display-device.cpp)
 * - if the display was ON and nothing was scheduled (SPENTO): noop
 * - if the display was ON and there was an ongoing schedule: just increase or
 *    decrease the target T
 */
void SchedulerDomain::_onTargetTRotaryRotation(
    pubsub_utils::TargetTRotaryRotationEvent* pEvent) {
  // If the rotary encoder was rotated when the display was OFF, then noop (as
  //  we have to switch on the display and NOT to increment the target T).
  if (pEvent->isDisplayOn) {
    if (isScheduled()) {
      // If there is an ongoing schedule, then increase/decrease the target T.
      targetTemperature = pEvent->value;
      pubsub_utils::pubSub.publish(
          new pubsub_utils::SchedulerTargetTUpdateEvent(targetTemperature));
    }
  }
}

void SchedulerDomain::reset() {
  bool wasScheduled = isScheduled();
  timer.reset();
  targetTemperature = settings::INITIAL_TARGET_T;
  if (wasScheduled)
    pubsub_utils::pubSub.publish(new pubsub_utils::SchedulerEndEvent());
}

struct time_utils::Time SchedulerDomain::tick() {
  // If the time is already over, then no need to publish the event.
  if (timer.isOver()) return timer.getTime();
  struct time_utils::Time time = timer.tick();
  if (timer.isOver())
    pubsub_utils::pubSub.publish(new pubsub_utils::SchedulerEndEvent());
  return time;
}

}  // namespace tstat