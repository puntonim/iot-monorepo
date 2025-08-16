#include "errors.h"

#include "utils/pubsub-utils.h"

namespace incub {

// "Soft" singleton global object defined here,
//  but defined as extern and initialized in errors.h.
ErrorManager errorManager;

void ErrorManager::_handleErrors() {
  // noop.
}

void ErrorManager::addDs18b20SensorError() {
  // Add the error msg to the errors list (only if we weren't already in the
  // same error state).
  if (_isDs18b20SensorError) return;
  _errorMessageListForDisplay.push_back(_DS18B20_SENSOR_ERROR_MSG);
  // Set the error status to true only after the prev check.
  _isDs18b20SensorError = true;
  pubsub_utils::pubSub.publish(new pubsub_utils::ErrorStatusUpdateEvent(
      true, _errorMessageListForDisplay));
  _handleErrors();
}

void ErrorManager::removeDs18b20SensorError() {
  // Remove the error msg from the errors list (only if we were in the same
  // error state).
  if (!_isDs18b20SensorError) return;
  _errorMessageListForDisplay.remove(_DS18B20_SENSOR_ERROR_MSG);
  // Set the error status to false only after the prev check.
  _isDs18b20SensorError = false;
  if (!areThereErrors()) {
    pubsub_utils::pubSub.publish(
        new pubsub_utils::ErrorStatusUpdateEvent(false));
  }
  _handleErrors();
}

bool ErrorManager::isDs18b20SensorError() { return _isDs18b20SensorError; }

void ErrorManager::addSht85SensorError() {
  // Add the error msg to the errors list (only if we weren't already in the
  // same error state).
  if (_isSht85SensorError) return;
  _errorMessageListForDisplay.push_back(_SHT85_SENSOR_ERROR_MSG);
  // Set the error status to true only after the prev check.
  _isSht85SensorError = true;
  pubsub_utils::pubSub.publish(new pubsub_utils::ErrorStatusUpdateEvent(
      true, _errorMessageListForDisplay));
  _handleErrors();
}

void ErrorManager::removeSht85SensorError() {
  // Remove the error msg from the errors list (only if we were in the same
  // error state).
  if (!_isSht85SensorError) return;
  _errorMessageListForDisplay.remove(_SHT85_SENSOR_ERROR_MSG);
  // Set the error status to false only after the prev check.
  _isSht85SensorError = false;
  if (!areThereErrors()) {
    pubsub_utils::pubSub.publish(
        new pubsub_utils::ErrorStatusUpdateEvent(false));
  }
  _handleErrors();
}

bool ErrorManager::isSht85SensorError() { return _isSht85SensorError; }

bool ErrorManager::areThereErrors() {
  // Check for the existence of AT LEAST 1 error.
  if (isDs18b20SensorError() || isSht85SensorError())
    return true;
  else
    return false;
}

/**
 * Get all the error messages as a list to be printed on the 2nd row of the
 *  16x2 display.
 */
std::list<char*> ErrorManager::getErrorMessageListForDisplay() {
  return _errorMessageListForDisplay;
}

}  // namespace incub