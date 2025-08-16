// Disables static receiver code, like receive timer ISR handler and static
//  IRReceiver and irparams data. Saves 450 bytes program memory and 269 bytes
//  RAM. This is due to a bug in RISC-V compiler, which requires unused function
//  sections.
#if !defined(ARDUINO_ESP32C3_DEV)
#define DISABLE_CODE_FOR_RECEIVER
#endif

#include <Arduino.h>
#include <ArduinoHttpClient.h>
#include <SHTSensor.h>
#include <TaskManagerIO.h>
#include <WiFiS3.h>

#include <IRremote.hpp>

#define SENSOR_ERROR -127
#define SENSOR_NULL -128

// Functions declaration.
void printWifiConnectionInfo();
void getSht85Data(float* data);
void postClimateData(float* data);
void postRemoteLog(const char* message);
void run();
void switchAc(float* data);

char ssid[] = "XXX";  // TODO change to the real values.
char pass[] = "XXX";  // TODO change to the real values.
WiFiClient wifiClient;
WiFiSSLClient wifiHttpsClient;
taskid_t runTaskId = TASKMGR_INVALIDID;
// This should be done only once, in case of many requests.
HttpClient client = HttpClient(wifiClient, IPAddress(192, 168, 1, 251), 7777);

// Note: SHT85 I2C address: 0x44.
SHTSensor sht(SHTSensor::SHT85);
// To use autodetect: SHTSensor sht;

const unsigned short IR_LED_PIN = 2;

unsigned long lastClimateDataPostTs = 0;
unsigned long lastAcSwitchTs = 0;
bool isAcStatusOn = false;

const float targetT = 26.0;
const float targetH = 57.0;
// const float targetT = 23.0;
// const float targetH = 55.0;

void setup() {
  Serial.begin(57600);
  unsigned short i = 30;
  while (!Serial && i--) delay(1);
  Serial.println("START " __FILE__ " compiled on " __DATE__);

  // ** Setup SHT85 sensor **.
  Wire.begin();
  // It should return a bool, but I tested it disconnecting wires and it always
  //  returns 0;
  sht.init();
  // SHT_ACCURACY_HIGH: highest repeatability at the cost of slower measurement.
  sht.setAccuracy(SHTSensor::SHT_ACCURACY_HIGH);

  // ** Setup IR LED emitter **.
  IrSender.begin(IR_LED_PIN);
  // Disable feedback on the default LED.
  disableLEDFeedback();

  // ** Setup WiFi **.
  // Ensure there is a WiFi module on the board.
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("ERROR WiFi module not detected!");
    Serial.println("Exiting...");
    while (true);
  }
  // Check the WiFi firmware version.
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println(
        "Please upgrade the firmware: "
        "https://support.arduino.cc/hc/en-us/articles/"
        "9670986058780-Update-the-connectivity-module-firmware-on-UNO-R4-WiFi");
  }
  // Connect to WiFi.
  unsigned short wifiConnectionStatus = WL_IDLE_STATUS;
  while (wifiConnectionStatus != WL_CONNECTED) {
    Serial.println((String) "Connecting to SSID " + ssid + "...");
    wifiConnectionStatus = WiFi.begin(ssid, pass);
    Serial.println("Waiting 10 secs...");
    delay(10 * 1000);
  }
  // Set the connection timeout (msec).
  wifiClient.setConnectionTimeout(3 * 1000);  // msec.
  wifiHttpsClient.setConnectionTimeout(3 * 1000);
  // Printing WiFi info.
  printWifiConnectionInfo();

  // ** Setup the period run **.
  // The first run is now, then schedule every 60 secs.
  run();
  runTaskId = taskManager.schedule(repeatSeconds(60), run);
}

void loop() { taskManager.runLoop(); }

float prevSht85Data[2] = {SENSOR_NULL, SENSOR_NULL};

void run() {
  unsigned long nowTs = millis();

  float sht85Data[2];
  getSht85Data(sht85Data);
  if (sht85Data[0] == SENSOR_ERROR) {
    return;
  }

  if (((nowTs - lastAcSwitchTs) > 2 * 60 * 1000) || (lastAcSwitchTs == 0)) {
    switchAc(sht85Data);
  }

  if ((nowTs - lastClimateDataPostTs) > 5 * 60 * 1000) {
    postClimateData(sht85Data);
  }

  prevSht85Data[0] = sht85Data[0];
  prevSht85Data[1] = sht85Data[1];
}

void switchAc(float* data) {
  float temp = data[0];
  float humidity = data[1];

  // Switch OFF case.
  if ((temp < targetT) && (humidity < targetH)) {
    if ((isAcStatusOn == false) && (prevSht85Data[0] < data[0]) &&
        (prevSht85Data[0] != SENSOR_NULL)) {
      return;
    }
    Serial.println("Sending command to 31 deg...");
    postRemoteLog("Sending command to 31 deg...");
    IrSender.sendPulseDistanceWidth(38, 9100, 4450, 650, 1600, 650, 550,
                                    0xD2030683, 48, PROTOCOL_IS_LSB_FIRST, 0,
                                    NO_REPEATS);
    isAcStatusOn = false;
    lastAcSwitchTs = millis();
    postClimateData(data);
  }
  // Switch ON case.
  else if ((temp >= targetT) || (humidity >= targetH)) {
    if ((isAcStatusOn == true) && (prevSht85Data[0] > data[0]) &&
        (prevSht85Data[0] != SENSOR_NULL)) {
      return;
    }
    Serial.println("Sending command to 22 deg...");
    postRemoteLog("Sending command to 22 deg...");
    IrSender.sendPulseDistanceWidth(38, 9050, 4450, 650, 1600, 650, 500,
                                    0x42030683, 48, PROTOCOL_IS_LSB_FIRST, 0,
                                    NO_REPEATS);
    isAcStatusOn = true;
    lastAcSwitchTs = millis();
    postClimateData(data);
  }
}

void getSht85Data(float* data) {
  if (sht.readSample()) {
    data[0] = sht.getTemperature();
    data[1] = sht.getHumidity();
  } else {
    data[0] = SENSOR_ERROR;
    data[1] = SENSOR_ERROR;
  }

  if (data[0] == SENSOR_ERROR) {
    Serial.println("ERROR reading data from SHT85");
    postRemoteLog("ERROR reading data from SHT85");
  } else {
    String msg = (String) "SHT85 reading: " + data[0] + "°C " + data[1] + "%";
    Serial.println(msg);
    postRemoteLog(msg.c_str());
  }
}

void postClimateData(float* data) {
  Serial.print("\tPOSTing climate data request... ");

  // Close any connection before sending a new request to free the socket on
  //  the NINA module.
  wifiClient.stop();

  // clang-format off
  char body[150];
  sprintf(
    body, 
    "{"
      "\"temp\": %2.2f,"
      "\"humidity\": %2.2f,"
      "\"room_id\": \"BEDROOM_PAOLO\","
      "\"app_id\": \"SMART_AC\","
      "\"device_id\": \"ARDU_LION\""
    "}",
    data[0],
    data[1]
  );
  // clang-format on
  client.post("/iot/temps/", "application/json", body);

  const unsigned short statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.println(statusCode);
  if ((statusCode != 200) && (statusCode != 201)) {
    Serial.println("ERROR received an HTTP error response");
    Serial.println((String) "Status code: " + statusCode);
    Serial.println("Response:\n" + response);
  }
  lastClimateDataPostTs = millis();
}

void postRemoteLog(const char* message) {
  Serial.print("\tPOSTing remote log request... ");

  // Close any connection before sending a new request to free the socket on
  //  the NINA module.
  wifiClient.stop();

  // clang-format off
  char body[150];
  sprintf(
    body, 
    "{"
      "\"message\": \"%s\","
      "\"app_id\": \"SMART_AC\","
      "\"device_id\": \"ARDU_LION\""
    "}",
    message
  );
  // clang-format on
  client.post("/iot/remote-logs/", "application/json", body);

  const unsigned short statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.println(statusCode);
  if ((statusCode != 200) && (statusCode != 201)) {
    Serial.println("ERROR received an HTTP error response");
    Serial.println((String) "Status code: " + statusCode);
    Serial.println("Response:\n" + response);
  }
}

void printWifiConnectionInfo() {
  Serial.print("Connected to SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("Assigned IP address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
