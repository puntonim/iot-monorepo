- the relay module sinks too much current and when on then the display is less bright
  However using a 80W USB charger solves the issue
  Try diff chargers and find the min power (mind W=VxA)
  Also, try removing the relay led and see if it gets better
  Also, try using a larger pull-down resistor for the led (current drained by the led
  should be less)

- Optionally disable the local temperature sensor (with a settings or #ifdef)?

- check scheduled tasks
- READY TO INSTALL AND USE

- rename project to "tsat"

- Bluetooth: communication between 2 Arduinos

- HW:

  - PSU for Arduino: 200V AC to USB 5V DC
    https://www.digikey.it/it/products/detail/mean-well-usa-inc/RS-15-5/7706168
    Or search amazon for "USB charger" or "macbook usb charger"
    like: https://www.amazon.it/Anker-Caricatore-Charger-compatto-incluso/dp/B0BDKSW8GC
  - 2x manual switces for thermostat wires and wires to Arduino's PSU

- Use PID algo in heating?
  https://playground.arduino.cc/Code/PIDLibrary/
  Or it is useless as the time it takes to reach the T is slow?
  Maybe I can just write some custom code such that the relay cannot change state in
  less than 5 minutes (to avoid hysteresis, ie. unwanted frequent switching).
  Then monitor how much it overshoots (what T does it reach after switching off) and
  how long it takes to reach the target T when it switches ON again.

- list-utils::getByIndex to set an IndexError
  using exceptions defined here: https://github.com/puntonim/arduino-monorepo/blob/790c425cb40c56209f74f5c541a8fc9b81dbcfd8/THERMOSTAT/thermostat-led-display/common-exceptions.h
  See notes in: /Volumes/home/Drive/DOCUMENTI-SYNC/IT/\_SW,\ SYS\ ENGINEERING/C++/Exceptions.md

- Optional args:

  - https://levelup.gitconnected.com/optional-arguments-in-c-f7e570dafdab
  - https://stackoverflow.com/questions/73421279/how-do-i-pass-and-modify-an-optional-argument-to-a-function-through-a-header-fil
    Then use it in time_utils::start() and note that I cannot import settings
    in time-utils.h cause it would be a circular dep, then I have to make the args
    optional and set them to the defaults if null in \*.cpp

- What to do in case of (local) sensor error?
  Maybe, in that error case, we should consider only the timer!
  But do we switch ON until the timer is over then switch OFF?
  or do we keep the current status (ON or OFF) until the timer is over and then
  switch OFF?

- What to do in case of remote sensor error?
  If we use a remote (Bluetooth) probe for the T, then the Bluetooth might fail
  or the sensor might fail. WHat to do?
  Do we also add an internal temp sensor?
  Or do we use the same logic as a local sensor error?

- Cloud and web:

  - send data to IoT cloud or custom BE to track:
    - T and H over time
    - heating ON/OFF
      These data could collected (in memory) and sent to the remote every 5 mins
      Or they could be collected every 1 min, stored in a SD card and sent to
      the remote every 5 mins
  - Hmmm: receive data from remote (IoT cloud? regular HTTP reqs?)
    But I don't rellay need the feature to switch ON/OFF the heating from
    remote

- Optimization: instead of cancelling tasks, enable/disable them:  
   `taskManager.setTaskEnabled(taskId, false);`
  Enabling a task, rather than scheduling it a new, is more efficient as it is O(1).
  Versus O(n) (n = #tasks) when sheduling it anew.
  However, later on, when we need to check if a task is enabled, we do:

  ```
  auto task = getTask(taskId);
  if (task.taskEnabled)...
  ```

  which is O(n) (n = #tasks).
  Versus O(1) in the current code where we just do:  
   `if (blinkTaskId == TASKMGR_INVALIDID)`
  So, in the end, there is probably zero-little performance gain.
