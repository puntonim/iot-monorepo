#include "devices/rotary-encoder-device.h"

#include <IoAbstraction.h>

#include "devices/display-device.h"
#include "settings.h"
#include "utils/pubsub-utils.h"

namespace incub {

// "Soft" singleton global object defined here,
//  but defined as extern and initialized in rotary-encoder-device.h.
RotaryEncoderDevices rotaryEncoderDevices;

/*
 * When any rotary encoder is pressed, execute this callback.
 *
 * Note: callbacks must be static functions (so this cannot be a regular class
 *  method) in order to be used in `switches.addSwitch()`.
 */
static void _onAnyRotaryPress(const uint8_t pin, const bool isHeldDown) {
  if (pin == settings::TARGET_T_ROTARY_SW_PIN)
    pubsub_utils::pubSub.publish(new pubsub_utils::TargetTRotaryPressEvent());
  else if (pin == settings::TIMER_ROTARY_SW_PIN) {
    pubsub_utils::pubSub.publish(new pubsub_utils::TimerRotaryPressEvent());
  }
}

void RotaryEncoderDevices::setup() {
  // Setup the rotary encoder press as a regular switch button.
  // Options: SWITCHES_NO_POLLING, SWITCHES_POLL_EVERYTHING,
  // SWITCHES_POLL_KEYS_ONLY (rotary encoders by interrupts, buttons by poll).
  // Mind that there is a limit in the number of devices that can work with
  //  interrupts simultaneously. If using SWITCHES_NO_POLLING with 2 encoders
  //  then the button press does not work, while SWITCHES_POLL_KEYS_ONLY works.
  // Last arg (true) is for integrated pull-up resistors.
  switches.init(ioUsingArduino(), SWITCHES_POLL_KEYS_ONLY, true);
  switches.addSwitch(settings::TARGET_T_ROTARY_SW_PIN, _onAnyRotaryPress,
                     NO_REPEAT);
  switches.addSwitch(settings::TIMER_ROTARY_SW_PIN, _onAnyRotaryPress,
                     NO_REPEAT);
}

}  // namespace incub
