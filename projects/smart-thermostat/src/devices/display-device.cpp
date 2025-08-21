#include "devices/display-device.h"

#include <LiquidCrystalIO.h>
// IoAbstractionWire.h required when using the I2C version.
#include <IoAbstractionWire.h>

#include "domain/scheduler-domain.h"
#include "errors.h"
#include "sensors/ds18b20-sensor.h"
#include "sensors/sht85-sensor.h"
#include "settings.h"
#include "utils/list-utils.h"
#include "utils/pubsub-utils.h"
#include "utils/task-manager-utils.h"
#include "utils/time-utils.h"

// This cannot be inside the namespace, not sure why.
LiquidCrystalI2C_RS_EN(lcd, 0x27, false);

namespace tstat {

//
//********** CLASS DisplayDevice ***********************************************

// "Soft" singleton global object defined here,
//  but defined as extern and initialized in led-device.h.
DisplayDevice displayDevice;

void DisplayDevice::setup() {
  Wire.begin();
  lcd.begin(16, 2);
  // Most I2C backpacks have the backlight on pin 3.
  lcd.configureBacklightPin(3, LiquidCrystal::BACKLIGHT_NORMAL);
  switchOn();

  // SUBSCRIPTION SchedulerEndEvent --------------------------------------------
  pubsub_utils::pubSub.subscribe([this](
                                     pubsub_utils::SchedulerEndEvent* pEvent) {
#if IS_DEBUG == true
    Serial.println((String) "DisplayDevice - received event: " + pEvent->topic);
#endif
    if (_isOn) this->_refreshFirstRow();
  });

  // SUBSCRIPTION TimerRotaryPressEvent ----------------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::TimerRotaryPressEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "DisplayDevice - received event: " +
                       pEvent->topic);
#endif
        // Switch ON the display if not already ON, cancel and reschedule
        //  the switch OFF task in 20 sec.
        this->switchOn();
      });

  // SUBSCRIPTION TimerRotaryRotationEvent -------------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::TimerRotaryRotationEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "DisplayDevice - received event: " +
                       pEvent->topic);
#endif
        // Switch ON the display if not already ON, cancel and reschedule
        //  the switch OFF task in 20 sec. Do not update the countdown timer
        //  on display as there will be a SchedulerTimerUpdateEvent for that.
        this->switchOn();
      });

  // SUBSCRIPTION TargetTRotaryPressEvent --------------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::TargetTRotaryPressEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "DisplayDevice - received event: " +
                       pEvent->topic);
#endif
        // Switch ON the display if not already ON, cancel and reschedule
        //  the switch OFF task in 20 sec.
        this->switchOn();
      });

  // SUBSCRIPTION TargetTRotaryRotationEvent -----------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::TargetTRotaryRotationEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "DisplayDevice - received event: " +
                       pEvent->topic);
#endif
        // Switch ON the display if not already ON, cancel and reschedule
        //  the switch OFF task in 20 sec.  Do not update the target T on
        //  display as there will be a SchedulerTargetTUpdateEvent for that.
        this->switchOn();
      });

  // SUBSCRIPTION SchedulerTimerUpdateEvent ------------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::SchedulerTimerUpdateEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "DisplayDevice - received event: " +
                       pEvent->topic);
#endif
        this->_refreshFirstRow();
      });

  // SUBSCRIPTION SchedulerTargetTUpdateEvent ----------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::SchedulerTargetTUpdateEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "DisplayDevice - received event: " +
                       pEvent->topic);
#endif
        _targetTemperature = pEvent->targetTemperature;
        this->_refreshFirstRow();
      });

  // SUBSCRIPTION SchedulerStartEvent ------------------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::SchedulerStartEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println((String) "DisplayDevice - received event: " +
                       pEvent->topic);
#endif
        _targetTemperature = schedulerDomain.targetTemperature;
        this->_refreshFirstRow();
      });

  // SUBSCRIPTION HeatingStatusUpdateEvent -------------------------------------
  pubsub_utils::pubSub.subscribe(
      [this](pubsub_utils::HeatingStatusUpdateEvent* pEvent) {
#if IS_DEBUG == true
        Serial.println(
            (String) "DisplayDevice - received event: " + pEvent->topic +
            " isOn=" + (pEvent->isOn ? "ON" : "OFF"));
#endif
        this->_isHeatingOn = pEvent->isOn;
        if (_isOn) this->_refreshFirstRow();
      });
}

void DisplayDevice::toogle() {
  if (_isOn == true)
    switchOff();
  else
    switchOn();
}
bool DisplayDevice::isOn() { return _isOn; }

void DisplayDevice::switchOff(bool doResetSwitchOffDisplayTaskId /* = true */) {
  lcd.noBacklight();
  lcd.noDisplay();
  _isOn = false;

  // We need to reset switchOffDisplayTaskId (or the display will always stay
  // on). No need to cancel any existing tasks to switch off the display because
  // we
  //  have just switched it off, so let those tasks run (but there shouldn't be
  //  any).
  if (doResetSwitchOffDisplayTaskId) switchOffTaskId = TASKMGR_INVALIDID;

  // And cancel any existing tasks to display data because no needed anymore.
#if IS_DEBUG == true
  Serial.println((String) "DisplayDevice - stopping the display task #" +
                 displayDataTaskId);
#endif
  task_manager_utils::cancelTask(displayDataTaskId);
  // And reset the counters.
  _counterForDisplayDataExecutions = 0;
  _indexForCurrentlyDisplayedErrorMsg = -1;
}

void DisplayDevice::switchOn(
    bool doCancelExistingSwitchOffDisplayTask /* = true */) {
  if (doCancelExistingSwitchOffDisplayTask) {
    // Cancel any existing tasks to switch off display because they have an old
    // schedule.
#if IS_DEBUG == true
    Serial.println((String) "DisplayDevice - stopping the switch off task #" +
                   switchOffTaskId);
#endif
    task_manager_utils::cancelTask(switchOffTaskId);
  }
  // And schedule a new task to switch off display.
#if IS_DEBUG == true
  Serial.println((
      String) "DisplayDevice - starting a new switch off "
              "task");
#endif
  switchOffTaskId =
      taskManager.schedule(onceSeconds(settings::DISPLAY_SWITCHOFF_TIMER),
                           [] { displayDevice.switchOff(); });

  // If the diplay is already ON, then nothing to do.
  if (!_isOn) {
    _isOn = true;
    _printData();
    lcd.backlight();
    lcd.display();
  }
}

/**
 * Display data on a 16x2 screen.
 *
 * It schedules itself to be run every 1 sec or so in order to update the
 * printed data.
 */
void DisplayDevice::_printData() {
  // Display the 2 rows.
  _printFirstRow();
  _printSecondRow();

  // We are done displaying data, so increment the executions counter.
  _counterForDisplayDataExecutions++;

  // Finally schedule a periodic task to update the data shown on display.
  // We cancel this task later on when the display is switched off.
  if ((displayDataTaskId == TASKMGR_INVALIDID) && _isOn) {
#if IS_DEBUG == true
    Serial.println(
        "DisplayDevice::_printData - starting a new "
        "display task");
#endif
    displayDataTaskId =
        taskManager.scheduleFixedRate(1000, [] { displayDevice._printData(); });
  }
}

void DisplayDevice::_printFirstRow() {
  RowPrinter p(0);

  schedulerDomain.tick();

  if (!schedulerDomain.isScheduled()) {
    p.print("     SPENTO");
    p.printFillingBlanks();
    return;
  }

  p.print(_targetTemperature);
  p.print("\xDF ");  // Or: p.print("\xDF""C");

  // Format time like: 1:04:09
  // Size 9 because of the final null appended by spritnf. And the hour can be 2
  // digits (eg, "26" hours)
  //  even if we don't do the 0-filling (so 1 hour is printed as "1" and not
  //  "01").
  char timerTimeString[9];
  // sprintf_P(timerTimeString, (PGM_P)F("%1d:%02d:%02d"), timerTime.h,
  // timerTime.m, timerTime.s);
  schedulerDomain.timer.format(timerTimeString);
  p.print(timerTimeString);
  p.print(" ");

  _isHeatingOn ? p.printRightAlign("ON") : p.printRightAlign("OFF");

  p.printFillingBlanks();
}

void DisplayDevice::_printSecondRow() {
  RowPrinter p(1);

  // If there are errors,
  //  and this is not the first execution after the button was pressed (when we
  //  have to print the regular row) then we need to show different error msgs
  //  every 3 seconds on rotation (which means that every 3 seconds we print the
  //  next error msg in the list).
  if (errorManager.areThereErrors() && (_counterForDisplayDataExecutions > 0)) {
    // Reminder division by 3, and do something only if reminder is 0.
    auto dv = std::div(_counterForDisplayDataExecutions, 3);
    if (dv.rem != 0)
      return;
    else {
      // It's time to rotate the msg on the display.
      short size = errorManager.getErrorMessageListForDisplay().size();
      // If we have already shown all the error msgs, then it's time to show the
      // regular row.
      if (_indexForCurrentlyDisplayedErrorMsg >= (size - 1)) {
        _indexForCurrentlyDisplayedErrorMsg = -1;
      } else {
        // Print the next msg in the list.
        _indexForCurrentlyDisplayedErrorMsg++;
        char* msg =
            list_utils::getByIndex(errorManager.getErrorMessageListForDisplay(),
                                   _indexForCurrentlyDisplayedErrorMsg);
        if (msg != nullptr) {
          p.print(msg);
          p.printFillingBlanks();
        } else {
          p.print("IndexError");
          p.printFillingBlanks();
#if IS_DEBUG == true
          Serial.println("IndexError in DisplayDevice::_printSecondRow");
#endif
        }
        return;
      }
    }
  }

  Ds18b20SensorException excDs;
  float t1 = ds18b20Sensor.getData(excDs);
  if (excDs != Ds18b20SensorException::Success) {
    p.print("???-");
  } else {
    // To use only 1 decimal.
    p.print(t1, 1);
    // How to print the degree symbol:
    //  https://forum.arduino.cc/t/solved-how-to-print-the-degree-symbol-extended-ascii/438685
    // lcd.print((String)F("\xDF") + "C");
    p.print("|");
  }

  Sht85SensorException excSh;
  struct Sht85Data sht85Data = sht85Sensor.getData(excSh);

  if (excSh != Sht85SensorException::Success) {
    p.print("???");
  } else {
    p.print(sht85Data.temperature, 1);
    // lcd.print((String)F("\xDF") + "C ");
    p.print("\xDF ");  // Or: p.print("\xDF""C ");
    // To use no decimals.
    p.print(sht85Data.humidity, 0);
    // To use 2 decimals.
    // lcd.print(sht85Data.humidity);
    // Blank filling the whole row that might contain text from the old error
    // screen.
    p.print("%");
  }
  p.printFillingBlanks();
}

void DisplayDevice::_refreshFirstRow() {
  if (!_isOn) return switchOn();
  task_manager_utils::cancelTask(displayDataTaskId);
  _printFirstRow();
  if ((displayDataTaskId == TASKMGR_INVALIDID) && _isOn) {
#if IS_DEBUG == true
    Serial.println(
        "DisplayDevice::_refreshFirstRow - starting a "
        "new display task");
#endif
    displayDataTaskId =
        taskManager.scheduleFixedRate(1000, [] { displayDevice._printData(); });
  }
}

//
//********** CLASS RowPrinter **************************************************

/*
 * Print data on a row.
 *
 * Note that you should first print the 1st row and then the 2nd row (or
 * viceversa). Printing a bit of the 1st row, then the 2nd row and then again a
 * bit of the 1st row will lead to bugs as the cursor would be in the wrong
 * position. To support this we should write more code.
 */
RowPrinter::RowPrinter(unsigned short i) {
  i = i;
  lcd.setCursor(0, i);
}

/**
 * Print something on the row.
 *
 * Note: this method actually use `lcd.print` which inherits from Arduino's
 *  class Print:
 * https://docs.arduino.cc/language-reference/en/functions/communication/print/
 * And it's defined twice to support both signatures. An alternative
 * implementation would be to inherit from Print.
 */
template <typename T>
void RowPrinter::print(T value) {
  size_t len = lcd.print(value);
  _currentSize += len;
}

/**
 * Print something on the row with the extra arg.
 *
 * Note: this method actually use `lcd.print` which inherits from Arduino's
 *  class Print:
 * https://docs.arduino.cc/language-reference/en/functions/communication/print/
 * And it's defined twice to support both signatures. An alternative
 * implementation would be to inherit from Print.
 */
template <typename T1, typename T2>
void RowPrinter::print(T1 value, T2 extra) {
  size_t len = lcd.print(value, extra);
  _currentSize += len;
}

/**
 * Fill the row with blanks.
 *
 * The purpose is to overwrite old text, longer than the one we are displaying.
 * Howerver this can hide some bugs (concurrent prints) so we might disable it
 *  on dev by setting DO_ENABLE_FILLING_BLANKS = false.
 */
void RowPrinter::printFillingBlanks() {
#if DO_ENABLE_FILLING_BLANKS == true
  short diff = _MAX_SIZE - _currentSize;
  for (int i = 0; i < diff; i++) lcd.print(" ");
  _currentSize = _MAX_SIZE;
#endif
}

/**
 * Print the given string to the rightmost part of the row.
 *
 * Note: it would be better to make this function generic (like print()) but
 *  then it's harder to compute the size of the string, as the string is the
 *  conversion of a generic type to string.
 */
void RowPrinter::printRightAlign(char* string) {
  short diff = _MAX_SIZE - _currentSize - strlen(string);
  for (int i = 0; i < diff; i++) lcd.print(" ");
  size_t len = lcd.print(string);
  _currentSize += len;
}

}  // namespace tstat
